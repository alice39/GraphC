#include <stdlib.h>
#include <string.h>

#include <map.h>

/**
 * Convert a key of a HashMap in a bucket index.
 *
 * @param map the hashmap where key belongs in
 * @param key the key to convert in an index
 * @return the index that belongs the key
 */
static inline msize_t _hashmap_buck_pos(const struct hashmap* map, mkey_t key);
/**
 * Indicate the actual load factor of a hashmap.
 *
 * @param map the hashmap where to get the load factor
 * @return the actual load factor
 */
static inline float _hashmap_load_factor(const struct hashmap* map);

/**
 * Add a node into a hashmap.
 *
 * @param map the hashmap where to add the node in
 * @param node the node to add
 * @param out_value the old value from an existing node
 * @param readonly if the node is read-only
 */
static void _hashmap_put(struct hashmap* map,
                         struct hashmap_node* node,
                         void** out_value,
                         bool readonly);
/**
 * Check and reserve more space in a hashmap.
 *
 * It'll reserve just if the load factor is getting
 * approximated to maximum load factor of hashmap.
 *
 * @see hashmap_reserve
 *
 * @param map the hashmap to reserve more space
 */
static void _hashmap_reserve(struct hashmap* map);
/**
 * Find a node with given key in a hashmap.
 *
 * @param map the hashmap where to seek the node
 * @param key the key linked to the node
 * @param out_back the back node, output NULL if there is no
 *                 out_back can be NULL to ignore it
 * @param out_node the found node, NULL if it doesn't exist
 */
static void _hashmap_find(const struct hashmap* map,
                          mkey_t key,
                          struct hashmap_node** out_back,
                          struct hashmap_node** out_node);

void hashmap_init(struct hashmap* map, msize_t initial_capacity, mfree_f destroyer) {
    if (map == NULL) {
        return;
    }

    hashmap_destroy(map);

    map->reserved = initial_capacity;
    map->size = 0;
    map->max_load_factor = MAP_DEFAULT_MAX_LOAD_FACTOR;
    map->destroyer = destroyer;
    map->bucks = calloc(initial_capacity, sizeof(struct hashmap_node*));

    // just if it failed to reserve space
    if (map->bucks == NULL) {
        map->reserved = 0;
    }
}

void hashmap_reserve(struct hashmap* map, msize_t capacity) {
    if (map == NULL || capacity == 0) {
        return;
    }

    msize_t new_size = map->reserved + capacity;
    struct hashmap_node** new_bucks = calloc(new_size, sizeof(struct hashmap_node*));
    if (new_bucks == NULL) {
        return;
    }

    msize_t size = map->size;

    msize_t old_reserved = map->reserved;
    struct hashmap_node** old_bucks = map->bucks;

    map->reserved = new_size;
    map->bucks = new_bucks;
    map->size = 0;

    // rehash all nodes in the new buck array

    for (size_t i = 0; i < old_reserved && size > 0; i++) {
        struct hashmap_node* node = old_bucks[i];
        if (node == NULL) {
            continue;
        }

        size--;

        struct hashmap_node* previous_node = node;
        struct hashmap_node* actual_node = node->next;

        for (; actual_node != NULL; actual_node = actual_node->next) {
            _hashmap_put(map, previous_node, NULL, false);
            previous_node = node;
        }

        _hashmap_put(map, previous_node, NULL, false);
    }
    
    free(old_bucks);
}

void hashmap_destroy(struct hashmap* map) {
    if (map == NULL) {
        return;
    }

    for (size_t i = 0; i < map->reserved && map->size > 0; i++) {
        struct hashmap_node* node = map->bucks[i];
        if (node == NULL) {
            continue;
        }

        map->size--;

        struct hashmap_node* previous_node = node;
        struct hashmap_node* actual_node = node->next;

        for (; actual_node != NULL; actual_node = actual_node->next) {
            if (map->destroyer != NULL) {
                map->destroyer(previous_node->entry.value);
            }
            free(previous_node);

            previous_node = actual_node;
        }

        if (map->destroyer != NULL) {
            map->destroyer(previous_node->entry.value);
        }
        free(previous_node);
    }

    map->reserved = 0;
    map->size = 0;
    map->destroyer = NULL;
    free(map->bucks);
    map->bucks = NULL;
}

msize_t hashmap_size(const struct hashmap* map) {
    if (map == NULL) {
        return 0;
    }

    return map->size;
}

void* hashmap_put(struct hashmap* map, mkey_t key, void* value) {
    if (map == NULL) {
        return NULL;
    }
    
    _hashmap_reserve(map);
    
    struct hashmap_node new_node = {
        .entry = {key, value},
        .next = NULL
    };

    void* old_value = NULL;
    _hashmap_put(map, &new_node, &old_value, true);
    return old_value;
}

bool hashmap_has(const struct hashmap* map, mkey_t key) {
    struct hashmap_node* node = NULL;
    _hashmap_find(map, key, NULL, &node);

    return node != NULL;
}

void* hashmap_get(const struct hashmap* map, mkey_t key) {
    struct hashmap_node* node = NULL;
    _hashmap_find(map, key, NULL, &node);

    return node != NULL ? node->entry.value : NULL;
}

void* hashmap_del(struct hashmap* map, mkey_t key) {
    struct hashmap_node* back_node = NULL;
    struct hashmap_node* node = NULL;
    _hashmap_find(map, key, &back_node, &node);

    if (node == NULL) {
        return NULL;
    }
    
    map->size--;

    void* value = node->entry.value;
    struct hashmap_node* next_node = node->next;

    if (back_node != NULL) {
        back_node->next = next_node;
    } else {
        map->bucks[_hashmap_buck_pos(map, key)] = next_node;
    }

    free(node);
    return value;
}

void hashmap_iterator_init(struct hashmap_iterator* it, const struct hashmap* map) {
    if (it == NULL) {
        return;
    }

    it->map = map;
    it->pos = 0;
    it->node = NULL;
    it->found_size = 0;
}

bool hashmap_iterator_next(struct hashmap_iterator* it, struct map_entry* out_entry) {
    if (it == NULL || it->map == NULL) {
        return false;
    }

    const struct hashmap_node* it_node = it->node;
    if (it_node != NULL) {
        it->node = it_node->next;
        it->found_size++;

        if (out_entry != NULL) {
            memcpy(out_entry, &it_node->entry, sizeof(struct map_entry));
        }

        return true;
    }

    const struct hashmap* map = it->map;
    for (; it->pos < map->reserved; it->pos++) {
        const struct hashmap_node* node = map->bucks[it->pos];
        if (node == NULL) {
            continue;
        }

        it->pos++;
        it->node = node->next;
        it->found_size++;

        if (out_entry != NULL) {
            memcpy(out_entry, &node->entry, sizeof(struct map_entry));
        }

        return true;
    }

    return false;
}

static inline msize_t _hashmap_buck_pos(const struct hashmap* map, mkey_t key) {
    if (map->reserved == 0) {
        return 0;
    }

    return key % map->reserved;
}

static inline float _hashmap_load_factor(const struct hashmap* map) {
    if (map->reserved == 0) {
        return 0;
    }

    return (float) map->size / map->reserved;
}

static void _hashmap_put(struct hashmap* map,
                         struct hashmap_node* node,
                         void** out_value,
                         bool readonly) {
    mkey_t key = node->entry.key;
    void* value = node->entry.value;

    struct hashmap_node* previous_buck = NULL;
    struct hashmap_node* buck = NULL;
    _hashmap_find(map, key, &previous_buck, &buck);

    if (buck != NULL) {
        if (out_value != NULL) {
            *out_value = buck->entry.value;
        }
        buck->entry.value = value;
        return;
    }

    map->size++;
    if (out_value != NULL) {
        *out_value = NULL;
    }

    struct hashmap_node* new_node = node;
    if (readonly) {
        new_node = malloc(sizeof(struct hashmap_node));
        new_node->entry.key = key;
        new_node->entry.value = value;
        new_node->next = NULL;
    }

    if (previous_buck != NULL) {
        previous_buck->next = new_node;
    } else {
        map->bucks[_hashmap_buck_pos(map, key)] = new_node;
    }
}

static void _hashmap_reserve(struct hashmap* map) {
    msize_t reserved = map->reserved;
    if (reserved == 0) {
        hashmap_reserve(map, 16);
        return;
    }

    if (map->max_load_factor > _hashmap_load_factor(map)) {
        return;
    }

    hashmap_reserve(map, reserved * 2);
}

static void _hashmap_find(const struct hashmap* map,
                          mkey_t key,
                          struct hashmap_node** out_back,
                          struct hashmap_node** out_node) {
    if (map == NULL || map->size == 0) {
        return;
    }

    msize_t buck_pos = _hashmap_buck_pos(map, key);
    struct hashmap_node* node = map->bucks[buck_pos];
    struct hashmap_node* back = NULL;

    while (node != NULL && node->entry.key != key) {
        back = node;
        node = node->next;
    }

    if (out_back != NULL) {
        *out_back = back;
    }
    
    *out_node = node;
}

