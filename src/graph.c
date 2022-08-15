#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <graph.h>
#include <list.h>

/**
 * Return the value that identifies that there is no an edge
 * in a graph.
 *
 * @param graph the graph to get the value
 * @return the empty value
 */
static inline int32_t g_empty_weight(const struct graph* graph);
/**
 * Check if two vertices belong in a graph.
 *
 * @param graph the graph where vertices belong in
 * @param vi the first vertex to check
 * @param wj the second vertex to check
 * @return true if at least one doesn't belong in given graph,
 *         otherwise false
 */
static inline bool g_is_out(const struct graph* graph, vertex_t vi, vertex_t wj);
/**
 * Check if initially the path between two vertices in
 * a graph.
 *
 * @param graph the graph to check the path
 * @param start_vertex the source vertex (where it starts at)
 * @param end_vertex the destination vertex (where it ends at)
 * @return true if it passes check, otherwise false
 */
static bool g_initial_path(struct graph* graph, vertex_t start_vertex, vertex_t end_vertex);
/**
 * Destroy a cache of a graph.
 *
 * @param graph the graph to destroy its cache
 */
static void g_invalidate_cache(struct graph* graph);

void graph_init(struct graph* graph, bool weighted, size_t len) {
    if (graph == NULL) {
        return;
    }

    graph_destroy(graph);

    graph->weighted = weighted;

    int32_t initial_value = g_empty_weight(graph);

    graph->len = len;
    graph->matrix = malloc(sizeof(int32_t*) * len);

    // initialize the adjacency matrix with the default
    // value of graph
    for (vertex_t i = 0; i < len; i++) {
        graph->matrix[i] = malloc(sizeof(int32_t) * len);
        for (vertex_t j = 0; j < len; j++) {
            graph->matrix[i][j] = initial_value;
        }
    }
}

void graph_print(const struct graph* graph) {
    if (graph == NULL) {
        return;
    }

    int32_t empty_weight = g_empty_weight(graph);

    for (vertex_t i = 0; i < graph->len; i++) {
        printf("(");
        for (vertex_t j = 0; j < graph->len; j++) {
            int32_t weight = graph->matrix[i][j];

            if (graph->weighted && weight == empty_weight) {
                printf("  -");
            } else {
                printf("%3d", weight);
            }

            if (j + 1 != graph->len) {
                printf(",");
            }
        }
        printf(")\n");
    }
}

void graph_destroy(struct graph* graph) {
    if (graph == NULL) {
        return;
    }
    
    graph->weighted = false;

    g_invalidate_cache(graph);

    if (graph->matrix != NULL) {
        for (vertex_t i = 0; i < graph->len; i++) {
            free(graph->matrix[i]);
        }
        free(graph->matrix);
    }

    graph->len = 0;
    graph->matrix = NULL;
}

void graph_addw(struct graph* graph, vertex_t vi, vertex_t wj, int32_t weight) {
    if (g_is_out(graph, vi, wj) || graph->matrix[vi][wj] == weight) {
        return;
    }
    
    g_invalidate_cache(graph);

    if (!graph->weighted && weight != 0) {
        weight = 1;
    }

    graph->matrix[vi][wj] = weight;
    graph->matrix[wj][vi] = weight;
}

void graph_add(struct graph* graph, vertex_t vi, vertex_t wj) {
    graph_addw(graph, vi, wj, 1);
}

bool graph_has(const struct graph* graph, vertex_t vi, vertex_t wj) {
    if (g_is_out(graph, vi, wj)) {
        return false;
    }

    int32_t weight = graph->matrix[vi][wj];
    int32_t empty_weight = g_empty_weight(graph);

    return weight != empty_weight;
}

int32_t graph_get(const struct graph* graph, vertex_t vi, vertex_t wj) {
    if (g_is_out(graph, vi, wj)) {
        return g_empty_weight(graph);
    }

    return graph->matrix[vi][wj];
}

void graph_del(struct graph* graph, vertex_t vi, vertex_t wj) {
    if (g_is_out(graph, vi, wj)) {
        return;
    }

    g_invalidate_cache(graph);

    int32_t empty_weight = g_empty_weight(graph);
    graph->matrix[vi][wj] = empty_weight;
    graph->matrix[wj][vi] = empty_weight;
}

size_t graph_rcount(const struct graph* graph, vertex_t vi) {
    if (g_is_out(graph, vi, 0)) {
        return 0;
    }

    size_t count = 0;
    int32_t empty_weight = g_empty_weight(graph);

    for (vertex_t j = 0; j < graph->len; j++) {
        if (graph->matrix[vi][j] != empty_weight) {
            count++;
        }
    }

    return count;
}

size_t graph_ccount(const struct graph* graph, vertex_t wj) {
    if (g_is_out(graph, 0, wj)) {
        return 0;
    }

    size_t count = 0;
    int32_t empty_weight = g_empty_weight(graph);

    for (vertex_t i = 0; i < graph->len; i++) {
        if (graph->matrix[i][wj] != empty_weight) {
            count++;
        }
    }

    return count;
}

void graph_wave(const struct graph* graph,
                vertex_t start_vertex,
                vertex_t end_vertex,
                bool should_duplicate,
                struct wave* out_wave) {
    if (g_is_out(graph, start_vertex, 0) || out_wave == NULL) {
        return;
    }

    // initialize the wave with the source vertex
    wave_init(out_wave, NULL, start_vertex);

    size_t vertex_len = graph->len;

    bool* visited = calloc(vertex_len, sizeof(bool));
    // inter_visited is used to be able duplicate the
    // vertices if there are several inter-connected between
    // themselves
    bool* inter_visited = calloc(vertex_len, sizeof(bool));

    // allow to track which wave belongs a vertex in a fast way
    vtwave_map wave_track = {0};
    hashmap_init(&wave_track, 0, NULL);
    hashmap_put(&wave_track, start_vertex, out_wave);

    struct queue_vertex wave_queue = {0};
    queue_vertex_init(&wave_queue);
    queue_vertex_add(&wave_queue, start_vertex);

    // to avoid adding more innecesary vertices
    bool found_vertex = false;

    while (!queue_vertex_empty(&wave_queue)) {
        size_t repeat_size = 1;

        if (should_duplicate) {
            repeat_size = queue_vertex_size(&wave_queue);
        }

        while (repeat_size > 0) {
            repeat_size--;

            vertex_t i = queue_vertex_del(&wave_queue);
            visited[i] = true;

            struct wave* wave = hashmap_get(&wave_track, i);

            for (vertex_t j = 0; j < vertex_len; j++) {
                if (visited[j] || !graph_has(graph, i, j)) {
                    continue;
                }

                inter_visited[j] = true;

                // add the vertex j as sub-wave of vertex i,
                // in same way, it'll be added the generated
                // sub-wave in the map.
                hashmap_put(&wave_track, j, wave_add(wave, j));

                found_vertex |= j == end_vertex;

                // if destination vertex was found
                // then it's not needed to queue more vertices
                if (found_vertex) {
                    continue;
                }

                queue_vertex_add(&wave_queue, j);
            }
        };

        for (size_t i = 0; i < vertex_len; i++) {
            visited[i] |= inter_visited[i];
            inter_visited[i] = false;
        }
    }
    
    free(visited); 
    free(inter_visited);

    hashmap_destroy(&wave_track);
    queue_vertex_destroy(&wave_queue);
}

void graph_components(struct graph* graph, const struct gcomponent** out_comp) {
    if (graph == NULL) {
        return;
    }

    struct gcomponent* cache_comp = graph->cache.component;
    // check if connected components were already computed
    // to avoid computing it again
    if (cache_comp != NULL) {
        if (out_comp != NULL) {
            *out_comp = cache_comp;
        }

        return;
    }

    // size of vertices that there are in the graph
    size_t vertex_len = graph->len;

    // indicates how many left vertices there are
    size_t remaining_len = vertex_len;
    // indicates which connected component belongs a vertex
    // this is structured as the following way:
    //     vertex_classes[vertex] = connected component ID
    // the connected component ID is invalid if it is 0
    uint32_t* vertex_classes = calloc(vertex_len, sizeof(uint32_t));

    // size of connected components exist
    // p by notation |G| = p
    size_t p = vertex_len;

    for (vertex_t i = 0; i < vertex_len && remaining_len > 0; i++) {
        // the vertex i already belongs in a connected component
        // or in other words, it already was visited
        if (vertex_classes[i] != 0) {
            continue;
        }

        // class_id represents the connected component ID
        uint32_t class_id = i + 1;
        vertex_classes[i] = class_id;

        for (vertex_t j = 0; j < vertex_len && remaining_len > 0; j++) {
            // if there is no edge (i, j) then it'll be ignored
            if (!graph_has(graph, i, j)) {
                continue;
            }
            
            // if it already belongs in the same connected
            // component, then ignore it
            if (vertex_classes[j] == class_id) {
                continue;
            }

            // decrease size of connected components that exist
            p--;

            // if it doesn't still belong in a connected
            // component then add it in one
            if (vertex_classes[j] == 0) {
                vertex_classes[j] = class_id;
                remaining_len--;
                continue;
            }

            // if the vertex j belongs in another connected
            // component then change all vertices that belong
            // in the actual connected component in the
            // connected component of vertex j

            uint32_t old_class_id = class_id;
            class_id = vertex_classes[j];

            for (vertex_t k = 0; k < vertex_len; k++) {
                if (vertex_classes[k] == old_class_id) {
                    vertex_classes[k] = class_id;
                }
            }
        }

        remaining_len--;
    }

    // generate a cache version to avoid recomputing twice
    // the connected components that belong the vertices

    cache_comp = calloc(1, sizeof(struct gcomponent));
    graph->cache.component = cache_comp;

    cache_comp->array.len = vertex_len;
    cache_comp->array.data = vertex_classes;

    // add the vertices that have the same ID in the same
    // vertex sequence
    u32vertices_map* map = &cache_comp->map;
    hashmap_init(map, p, u32vertices_destroyer);

    for (size_t i = 0; i < vertex_len; i++) {
        uint32_t i_class = vertex_classes[i];

        struct vertex_array* arr = hashmap_get(map, i_class);

        // si la secuencia de una componente conexa no existe
        // entonces crear una
        if (arr == NULL) {
            arr = calloc(1, sizeof(struct vertex_array));
            hashmap_put(map, i_class, arr);
        }

        // add the vertice at the end of sequence
        vertex_array_reserve(arr, 1);
        arr->data[arr->len++] = i;
    }

    if (out_comp != NULL) {
        *out_comp = cache_comp;
    }
}

bool graph_reachable(struct graph* graph, vertex_t start_vertex, vertex_t end_vertex) {
    if (g_is_out(graph, start_vertex, end_vertex)) {
        return false;
    }

    const struct gcomponent* component = NULL;
    graph_components(graph, &component);
    if (component == NULL) {
        return false;
    }

    // two vertices are reachable if they are in the same
    // connected component (or they have same ID)

    const uint32_t* data = component->array.data;
    return data[start_vertex] == data[end_vertex];
}

void graph_short_path(struct graph* graph,
                      vertex_t start_vertex,
                      vertex_t end_vertex,
                      u32vertices_map* out_map) {
    if (out_map == NULL || !g_initial_path(graph, start_vertex, end_vertex)) {
        return;
    }

    // generate the waves
    struct wave root_wave = {0};
    graph_wave(graph, start_vertex, end_vertex, true, &root_wave);

    // transform the waves into paths
    u32path_map paths = {0};
    wave_to_path(&root_wave, &paths);

    hashmap_init(out_map, hashmap_size(&paths), u32vertices_destroyer);
    mkey_t next_key = 0;

    // now filter all paths where the final vertex matches
    // with the destination vertex

    struct hashmap_iterator path_it = {0};
    hashmap_iterator_init(&path_it, &paths);
    for (struct map_entry entry; hashmap_iterator_next(&path_it, &entry);) {
        struct path* path = entry.value;
        struct vertex_array* vertices = &path->vertices;

        if (vertices->data[vertices->len - 1] != end_vertex) {
            continue;
        }

        struct vertex_array* clone_vertices = calloc(1, sizeof(struct vertex_array));
        vertex_array_clone(&path->vertices, clone_vertices);
        hashmap_put(out_map, next_key++, clone_vertices);
    }

    hashmap_destroy(&paths);
    wave_destroy(&root_wave);
}

void graph_minimal_path(struct graph* graph,
                          vertex_t start_vertex,
                          vertex_t end_vertex,
                          u32path_map* out_map) {
    if (out_map == NULL) {
        return;
    }
    if (end_vertex != VERTEX_T_MAX && !g_initial_path(graph, start_vertex, end_vertex)) {
        return;
    }

    const struct gcomponent* component = NULL;
    graph_components(graph, &component);
    if (component == NULL) {
        return;
    }

    size_t vertex_len = graph->len;

    bool* visited = calloc(vertex_len, sizeof(bool));
    // allow to track the minimal weight that given vertex
    // can can have, including its path
    struct path* minimal_paths = calloc(vertex_len, sizeof(struct path));

    // initialize with an invalid weight
    // (the maximum possible / infinity)
    for (size_t i = 0; i < vertex_len; i++) {
        struct path* path = &minimal_paths[i];

        path_init(path, NULL);
        path->weight = INT32_MAX;
    }

    // create an initial path for the source vertex
    // and add it in minimal_paths
    struct vertex_array initial_vertex = {0};
    vertex_array_from(&initial_vertex, (vertex_t[1]){start_vertex}, 1);

    struct path* start_path = &minimal_paths[start_vertex];
    path_init(start_path, &initial_vertex);
    start_path->weight = 0;

    struct queue_vertex queue = {0};
    queue_vertex_init(&queue);
    queue_vertex_add(&queue, start_vertex);

    while (!queue_vertex_empty(&queue)) {
        vertex_t i = queue_vertex_del(&queue);
        if (i == end_vertex) {
            continue;
        }

        visited[i] = true;

        // the actual path of vertex i
        struct path* i_path = &minimal_paths[i];

        // the vertices that have been accumulated
        struct vertex_array* accumulated_vertices = &i_path->vertices;
        // the weight accumulated of this path
        int32_t accumulated_distance = i_path->weight;

        for (vertex_t j = 0; j < vertex_len; j++) {
            if (visited[j] || !graph_has(graph, i, j)) {
                continue;
            }

            // the weight of edge <i, j>
            int32_t distance = graph_get(graph, i, j);
            // the absorbed weight of edge <i, j> and
            // the accumulated ones
            int32_t absorbed_distance = distance + accumulated_distance;

            struct path* j_path = &minimal_paths[j];

            // if a path was found to have the lower weight
            // then change it by this new path

            if (j_path->weight > absorbed_distance) {
                struct vertex_array* absorbed_vertices = &j_path->vertices;

                // clone the sequence model including this as
                // the end vertex
                vertex_array_clone(accumulated_vertices, absorbed_vertices);
                vertex_array_reserve(absorbed_vertices, 1);
                absorbed_vertices->data[absorbed_vertices->len++] = j;

                // the new lower weight found
                j_path->weight = absorbed_distance;
            }

            queue_vertex_add(&queue, j);
        }
    }

    // now add all generated paths in a map
    hashmap_init(out_map, 0, u32path_destroyer);

    for (vertex_t i = 0; i < vertex_len; i++) {
        struct path* path = &minimal_paths[i];
        // if it couldn't generate a path
        if (path->weight == INT32_MAX) {
            continue;
        }
        // if the length is 1, it means that is the
        // source vertex, so ignore it
        if (path->vertices.len == 1) {
            path_destroy(path);
            continue;
        }

        // move path into heap to not loose it
        struct path* heap_path = malloc(sizeof(struct path));
        memcpy(heap_path, path, sizeof(struct path));

        hashmap_put(out_map, i, heap_path);
    }

    free(visited);
    free(minimal_paths);
}

void gcomponent_destroy(struct gcomponent* comp) {
    if (comp == NULL) {
        return;
    }

    free(comp->array.data);
    comp->array.len = 0;
    comp->array.data = NULL;

    hashmap_destroy(&comp->map);
}

static inline int32_t g_empty_weight(const struct graph* graph) {
    if (graph == NULL) {
        return 0;
    }

    return graph->weighted ? NONE_WEIGHT32_VALUE : 0;
}

static inline bool g_is_out(const struct graph* graph, size_t vi, size_t wj) {
    return graph == NULL || vi >= graph->len || wj >= graph->len;
}

static bool g_initial_path(struct graph* graph, vertex_t start_vertex, vertex_t end_vertex) {
    if (g_is_out(graph, start_vertex, end_vertex)) {
        return false;
    }

    // Check if two vertices are reachable, such that,
    // exist at least a path to arrive a vertex from
    // another one
    return graph_reachable(graph, start_vertex, end_vertex);
}

static void g_invalidate_cache(struct graph* graph) {
    struct gcomponent* component = graph->cache.component;
    gcomponent_destroy(component);
    free(component);

    graph->cache.component = NULL;
}

