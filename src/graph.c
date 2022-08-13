#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/graph.h"
#include "../include/list.h"

/**
 * Devuelve el valor que identifica que no hay un vinculo en
 * un grafo.
 *
 * @param graph el grafo a obtener el valor
 * @return el valor vacio
 */
static inline int32_t g_empty_weight(const struct graph* graph);
/**
 * Verifica si dos vertices no pertenen al grafo.
 *
 * @param graph el grafo donde pertenen los vertices
 * @param vi el primer vertice a comprobar
 * @param wj el segundo vertice a comprobar
 * @return true si no pertenen al grafo de lo contrario false 
 */
static inline bool g_is_out(const struct graph* graph, vertex_t vi, vertex_t wj);
/**
 * Verifica inicicialmente el camino de dos vertice en el
 * grafo.
 *
 * @param graph el grafo a verificar el camino
 * @param start_vertex el vertice de partida del camino
 * @param end_vertex el vertice de llegada del camino
 * @return true si todo está correcto, de lo contrario false
 */
static bool g_initial_path(struct graph* graph, vertex_t start_vertex, vertex_t end_vertex);
/**
 * Destruye la cache de un grafo.
 *
 * @param graph el grafo a destruir la memoria cache
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

    // inicializa la matriz de adyadencia con el valor
    // por defecto del grafo
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

// NOTA: no es usado, lo implementé por las clases
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

// NOTA: no es usado, pero lo implementé por las clases
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

    // inicializa la onda con el vertice de partida
    wave_init(out_wave, NULL, start_vertex);

    size_t vertex_len = graph->len;

    bool* visited = calloc(vertex_len, sizeof(bool));
    // inter_visited es usado para poder duplicar los
    // vertices si hay varios vertices inter-conectados
    // entre si
    bool* inter_visited = calloc(vertex_len, sizeof(bool));

    // permite localizar que onda pertene un vertice de forma
    // rápida y eficiente
    vtwave_map wave_track = {};
    hashmap_init(&wave_track, 0, NULL);
    hashmap_put(&wave_track, start_vertex, out_wave);

    struct queue_vertex wave_queue = {};
    queue_vertex_init(&wave_queue);
    queue_vertex_add(&wave_queue, start_vertex);

    // para evitar de añadir mas vertices innecesarios
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

                // añade el vertice j como la subonda del
                // vertice i, de misma forma se añade la
                // onda generada del vertice j en el mapa
                hashmap_put(&wave_track, j, wave_add(wave, j));

                found_vertex |= j == end_vertex;

                // si ya se encontró el vertice final
                // ya no es necesario encolar más vertices
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
    // verificar si las componentes conexas ya han sido
    // calculadas para evitar recalcularla
    if (cache_comp != NULL) {
        if (out_comp != NULL) {
            *out_comp = cache_comp;
        }

        return;
    }

    // cantidad de vertices que hay en el grafo
    size_t vertex_len = graph->len;

    // indica cuantos vertices faltan para recorrer
    size_t remaining_len = vertex_len;
    // indica en que componente conexa pertenene un vertice
    // esto está estructurado de la siguiente manera:
    //     vertex_classes[vertice] = ID de componente conexa
    // la ID de una componente conexa invalida es 0
    uint32_t* vertex_classes = calloc(vertex_len, sizeof(uint32_t));

    // numeros de componentes conexas que existen
    // p por notación |G| = p
    size_t p = vertex_len;

    for (vertex_t i = 0; i < vertex_len && remaining_len > 0; i++) {
        // el vertice i ya pertenen a una componente conexa
        // o en otras palabras, ya fue visitado
        if (vertex_classes[i] != 0) {
            continue;
        }

        uint32_t class_id = i + 1;
        vertex_classes[i] = class_id;

        // class_id representa la ID de la componente conexa
        // el nombre class_id es debido a clase de equivalencia
        // uint32_t class_id = i + 1;
        // vertex_classes[i] = class_id;

        for (vertex_t j = 0; j < vertex_len && remaining_len > 0; j++) {
            // si no hay un vinculo (i, j) entonces se ignorará
            if (!graph_has(graph, i, j)) {
                continue;
            }
            
            // si ya pertenen a la misma componente conexa,
            // entonces ignorarlo
            if (vertex_classes[j] == class_id) {
                continue;
            }

            // se reduce la cantidad de componentes conexas
            // que existen
            p--;

            // si todavía no pertenen a ninguna componente
            // conexa entonces añadirlo a una
            if (vertex_classes[j] == 0) {
                vertex_classes[j] = class_id;
                remaining_len--;
                continue;
            }

            // si el vertice j ya pertenecia a otra componente
            // conexa entonces hacer que todos los vertices de
            // la actual componente conexa se vuelvan de la
            // misma que ya pertenecia el vertice

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

    // generar una version cache para evitar recalcular
    // de nuevo las componentes conexas que pertenecen
    // los vertices

    cache_comp = calloc(1, sizeof(struct gcomponent));
    graph->cache.component = cache_comp;

    cache_comp->array.len = vertex_len;
    cache_comp->array.data = vertex_classes;

    // añade los vertices que tengan la misma ID de la
    // componente conexa en una misma secuencia/lista
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

        // añade el vertice al final de la secuencia
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

    // dos vertices son alcanzable si están en la misma
    // componente conexa (o tienen la misma ID)

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

    // genera las ondas
    struct wave root_wave = {};
    graph_wave(graph, start_vertex, end_vertex, true, &root_wave);

    // convierte las ondas en camino
    u32path_map paths = {};
    wave_to_path(&root_wave, &paths);

    hashmap_init(out_map, hashmap_size(&paths), u32vertices_destroyer);
    mkey_t next_key = 0;

    // ahora añade todas las rutas que contenga solamente
    // el vertice final

    struct hashmap_iterator path_it = {};
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
    // permite localizar el costo/peso minimo que dado
    // vertice puede tener incluyendo su ruta
    struct path* minimal_paths = calloc(vertex_len, sizeof(struct path));

    // inicializa con un costo invalido (el mayor posible/infinito)
    for (size_t i = 0; i < vertex_len; i++) {
        struct path* path = &minimal_paths[i];

        path_init(path, NULL);
        path->weight = INT32_MAX;
    }

    // crea la ruta inicial para el vertice de partida
    // y añadirla al arreglo minimal_paths
    struct vertex_array initial_vertex = {};
    vertex_array_from(&initial_vertex, (vertex_t[1]){start_vertex}, 1);

    struct path* start_path = &minimal_paths[start_vertex];
    path_init(start_path, &initial_vertex);
    start_path->weight = 0;

    struct queue_vertex queue = {};
    queue_vertex_init(&queue);
    queue_vertex_add(&queue, start_vertex);

    while (!queue_vertex_empty(&queue)) {
        vertex_t i = queue_vertex_del(&queue);
        if (i == end_vertex) {
            continue;
        }

        visited[i] = true;

        // la ruta actual del vertice i
        struct path* i_path = &minimal_paths[i];

        // los vertices que han sido acomulado
        struct vertex_array* accumulated_vertices = &i_path->vertices;
        // el costo/peso acomulado de esta ruta
        int32_t accumulated_distance = i_path->weight;

        for (vertex_t j = 0; j < vertex_len; j++) {
            if (visited[j] || !graph_has(graph, i, j)) {
                continue;
            }

            // el costo/peso entre el arco <i, j>
            int32_t distance = graph_get(graph, i, j);
            // el costo/peso absorbido entre el arco <i, j>
            // y los acomulados
            int32_t absorbed_distance = distance + accumulated_distance;

            struct path* j_path = &minimal_paths[j];

            // si se encontró una ruta con el menor costo/peso
            // actual, cambiarla por la nueva que se ha generado

            if (j_path->weight > absorbed_distance) {
                struct vertex_array* absorbed_vertices = &j_path->vertices;

                // hacer una copia modelo de los vertices
                // incluyendo este al final
                vertex_array_clone(accumulated_vertices, absorbed_vertices);
                vertex_array_reserve(absorbed_vertices, 1);
                absorbed_vertices->data[absorbed_vertices->len++] = j;

                // el costo/peso minimo encontrado
                j_path->weight = absorbed_distance;
            }

            queue_vertex_add(&queue, j);
        }
    }

    // Ahora añadir todos las rutas generadas a un mapa
    hashmap_init(out_map, 0, u32path_destroyer);

    for (vertex_t i = 0; i < vertex_len; i++) {
        struct path* path = &minimal_paths[i];
        // si no se pudo generar un camino hasta aquí
        if (path->weight == INT32_MAX) {
            continue;
        }
        // Si la longitud es 1, indica que es el vertice
        // de partida, así que ignorarlo
        if (path->vertices.len == 1) {
            path_destroy(path);
            continue;
        }

        // hacer una copia en el heap de la ruta
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

    // Verificar si los vertices son alcanzable, es decir,
    // existe al menos un camino para llegar a un vertice
    // a otro
    return graph_reachable(graph, start_vertex, end_vertex);
}

static void g_invalidate_cache(struct graph* graph) {
    struct gcomponent* component = graph->cache.component;
    gcomponent_destroy(component);
    free(component);

    graph->cache.component = NULL;
}

