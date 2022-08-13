#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <graph.h>
#include <list.h>

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
        arr->data[arr->len - 1] = i;
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

    if (start_vertex == end_vertex) {
        struct vertex_array* path = calloc(1, sizeof(struct vertex_array));
        vertex_array_from(path, (vertex_t[1]) {start_vertex}, 1);

        hashmap_init(out_map, 1, u32vertices_destroyer);
        hashmap_put(out_map, 0, path);
        return;
    }

    size_t vertex_len = graph->len;

    bool* visited = calloc(vertex_len, sizeof(bool));
    vertex_t* waves = malloc(sizeof(vertex_t) * vertex_len * vertex_len);

    // comienza la primera ola con el vertice de partida
    waves[0] = start_vertex;
    // inicializa las olas con un vertice invalido
    for (size_t i = 1; i < vertex_len * vertex_len; i++) {
        waves[i] = VERTEX_T_MAX;
    }
    
    // indice en que momento la ola terminó
    size_t final_wave = 0;

    for (size_t i_wave = 0; i_wave < vertex_len - 1; i_wave++) {
        if (final_wave != 0) {
            break;
        }

        size_t j_wave = 0;

        // recorre todos los vertices de la ola actual
        vertex_t* actual_waves = &waves[i_wave * vertex_len];
        for (vertex_t i_vertex = *actual_waves; i_vertex != VERTEX_T_MAX; i_vertex = *actual_waves++) {
            if (final_wave != 0) {
                break;
            }
            visited[i_vertex] = true;

            for (vertex_t j_vertex = 0; j_vertex < vertex_len; j_vertex++) {
                if (visited[j_vertex] || !graph_has(graph, i_vertex, j_vertex)) {
                    continue;
                }

                visited[j_vertex] = true;
                // añade el vertice para la siguiente ola
                waves[j_wave + (i_wave + 1) * vertex_len] = j_vertex;
                j_wave++;

                // si se encontró el vertice final
                if (j_vertex == end_vertex) {
                    final_wave = i_wave;
                    break;
                }
            }
        }
    }

    struct vertex_array* initial_vertices = calloc(1, sizeof(struct vertex_array));
    vertex_array_reserve(initial_vertices, 1);
    initial_vertices->data[0] = end_vertex;

    hashmap_init(out_map, 0, u32vertices_destroyer);
    hashmap_put(out_map, 0, initial_vertices);

    for (size_t i = 0; i< vertex_len; i++) {
        visited[i] = false;
    }

    // Coloca todos los caminos posibles en una lista de
    // secuencia
    for (size_t i_wave = final_wave; i_wave > 0; i_wave--) {
        // la nueva lita generada
        u32vertices_map new_map = {};
        hashmap_init(&new_map, hashmap_size(out_map), u32vertices_destroyer);

        mkey_t next_key = 0;

        struct hashmap_iterator it = {};
        hashmap_iterator_init(&it, out_map);
        for (struct map_entry entry; hashmap_iterator_next(&it, &entry);) {
            // top es la secuencia modelo para futuros caminos
            struct vertex_array* top = entry.value;
            // el nuevo vertice final
            vertex_t next_end_vertex = top->data[top->len - 1];

            vertex_t* actual_waves = &waves[i_wave * vertex_len];
            for (vertex_t i_vertex = *actual_waves; i_vertex != VERTEX_T_MAX; i_vertex = *actual_waves++) {
                if (visited[i_vertex] || !graph_has(graph, i_vertex, next_end_vertex)) {
                    continue;
                }

                visited[i_vertex] = true;

                // clona la secuencia modelo en una nueva
                struct vertex_array* top_clone = calloc(1, sizeof(struct vertex_array));
                vertex_array_clone(top, top_clone);

                // añade el vertice al final de la secuencia
                vertex_array_reserve(top_clone, 1);
                top_clone->data[top_clone->len - 1] = i_vertex;

                // coloca la nueva secuencia en la nueva lista
                hashmap_put(&new_map, next_key++, top_clone);
            }
        }

        // elimina la vieja lista y se cambia por la nueva
        // lista que se ha generado
        hashmap_destroy(out_map);
        memcpy(out_map, &new_map, sizeof(u32vertices_map));
    }

    free(visited);
    free(waves);

    // Finalmente añadir el vertice de incio
    // y darle la vuelta al arreglo, ya que
    // se hizo de forma regresiva desde el
    // vertice final hasta el vertice de inicio

    struct hashmap_iterator it = {};
    hashmap_iterator_init(&it, out_map);
    for (struct map_entry entry; hashmap_iterator_next(&it, &entry);) {
        struct vertex_array* path = entry.value;

        vertex_array_reserve(path, 1);
        path->data[path->len - 1] = start_vertex;

        vertex_array_backwards(path);
    }
}

void graph_minimal_path(struct graph* graph,
                          vertex_t start_vertex,
                          vertex_t end_vertex,
                          u32path_map* out_map) {
    if (out_map == NULL || !g_initial_path(graph, start_vertex, end_vertex)) {
        return;
    }

    const struct gcomponent* component = NULL;
    graph_components(graph, &component);
    if (component == NULL) {
        return;
    }

    hashmap_init(out_map, 0, u32path_destroyer);

    size_t vertex_len = graph->len;
    uint32_t* vertex_costs = malloc(sizeof(uint32_t) * vertex_len);

    // inicializa con un costo invalido (el mayor posible/infinito)
    for (size_t i = 1; i < vertex_len; i++) {
        vertex_costs[i] = UINT32_MAX;
    }

    for (vertex_t i = 0; i < vertex_len; i++) {
        for (vertex_t j = 0; j < vertex_len; j++) {

        }
    }
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

void gpath_destroy(struct gpath* path) {
    if (path == NULL) {
        return;
    }

    vertex_array_destroy(&path->vertices);
    path->weight = 0;
}

void gwave_destroyer(void* wave) {
    gwave_destroy(wave);
    free(wave);
}

void u32path_destroyer(void* path) {
    gpath_destroy(path);
    free(path);
}

