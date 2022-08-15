#ifndef ED_HASHMAP_GUARD_HEADER
#define ED_HASHMAP_GUARD_HEADER

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * The maximum load factor by default.
 */
#define MAP_DEFAULT_MAX_LOAD_FACTOR 1.0f

/**
 * Represents they map key.
 */
typedef uint64_t mkey_t;
/**
 * Represents the map size.
 */
typedef size_t msize_t;
/**
 * Represents the value destructor of a map.
 */
typedef void (*mfree_f)(void* value);

/**
 * Represents a pair that relates a key with a value.
 *
 * @member key the used key
 * @member value the stored value
 */
struct map_entry {
    mkey_t key;
    void* value;
};

/**
 * Represents a node of a struct map_entry in a double linked
 * list.
 *
 * The node is used to avoid collisions in the map.
 *
 * @member entry the pair that relates a key with a value
 * @member next the next node (underlying on this node), NULL
 *              if there is no (then this is the last node)
 */
struct hashmap_node {
    struct map_entry entry;

    struct hashmap_node* next;
};

/**
 * The data structure that represents a HashMap.
 *
 * Seen in another way: HashMap<Key, Value>
 *
 * @see hashmap_init
 * @see hashmap_destroy
 *
 * @member reserved the length of space reserved
 * @member size the length of values added
 * @member max_load_factor the factor that indicates when it
 *                         should increase the reserved space
 * @member destroyer the function that destroyes the stored
 *                   value in the map, it can be NULL if it's
 *                   not needed
 * @member bucks is the array that contains all nodes
 */
struct hashmap {
    msize_t reserved;
    msize_t size;
    float max_load_factor;

    mfree_f destroyer;

    struct hashmap_node** bucks;
};

/**
 * A HashMap iterator.
 *
 * @see hashmap_iterator_init
 *
 * @member map the map where it's iterating on
 * @member pos the next position to iterate
 * @member node the next node to iterate, NULL if there is no
 * @member found_size the size of elements that have been found
 *                    along actual iteration
 */
struct hashmap_iterator {
    const struct hashmap* map;
    msize_t pos;
    const struct hashmap_node* node;

    msize_t found_size;
};

/**
 * Initialize a HashMap with a initial capacity.
 *
 * @param map the hashmap to initialize
 * @param initial_capacity capacity to reserve
 * @param destroyer the value destructor, it can be NULL
 */
void hashmap_init(struct hashmap* map, msize_t initial_capacity, mfree_f destroyer);
/**
 * Reserve more capacity to a hashmap.
 *
 * If the capacity is 0, then it'll not apply any action.
 *
 * @param map the hashmap to reserve space
 * @param capacity the capacity to reserve
 */
void hashmap_reserve(struct hashmap* map, msize_t capacity);
/**
 * Destroy an initialized hashmap.
 *
 * @param map the hashmap to destroy
 */
void hashmap_destroy(struct hashmap* map);

/**
 * The size of values added in the hashmap.
 *
 * @param map the hashmap to get the size
 * @return the size of values that there is
 */
msize_t hashmap_size(const struct hashmap* map);

/**
 * Add a value with a given key in a hashmap.
 *
 * @param map the hashmap to add object pair
 * @param key the key to link with the value
 * @param value the value to store
 * @return the previous value that was stored with the key
 */
void* hashmap_put(struct hashmap* map, mkey_t key, void* value);
/**
 * Check if a key is added in a hashmap.
 *
 * @param map the hashmap to look for the key
 * @param key they key to check
 * @return true if it is added, otherwise false
 */
bool hashmap_has(const struct hashmap* map, mkey_t key);
/**
 * Return the value linked with a key in a hashmap.
 *
 * If it doesn't exist any key in the hashmap, NULL weill be
 * returned.
 *
 * @see hashmap_has
 *
 * @param map the hashmap to look for the value
 * @param key the key linked with the value
 * @return the found value
 */
void* hashmap_get(const struct hashmap* map, mkey_t key);
/**
 * Delete a key from a hashmap.
 *
 * @param map the hashmap where to delete the key on
 * @param key the key to look for and delete
 * @return the linked value with the key
 */
void* hashmap_del(struct hashmap* map, mkey_t key);

/**
 * Initialize a hashmap iterator.
 *
 * @param it the iterator to initilize
 * @param map the hashmap that will be iterated
 */
void hashmap_iterator_init(struct hashmap_iterator* it, const struct hashmap* map);
/**
 * Iterate to the next element.
 *
 * @param it the iterator where it'll look for the next element
 * @param out_entry the found element
 * @return true if it cound find another element, otherwise false
 */
bool hashmap_iterator_next(struct hashmap_iterator* it, struct map_entry* out_entry);

#endif // ED_MAP_GUARD_HEADER
