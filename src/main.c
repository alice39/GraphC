#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <graph.h>

/**
 * Read from a file data of a graph.
 *
 * @param graph the graph to store the data
 * @param file the file to read
 */
static void fread_graph(struct graph* graph, FILE* file);
/**
 * The main menu.
 *
 * @param graph the graph
 */
static void on_menu(struct graph* graph);

int main(int argc, char** args) {
    const char* filename = argc > 1 ? args[1] : "Panas.in";
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("The file %s doesn't exist or cannot be read\n", filename);
        return 1;
    }

    struct graph graph = {0};

    printf("Loading file: %s\n", filename);
    fread_graph(&graph, file);
    fclose(file);

    on_menu(&graph);

    graph_destroy(&graph);

    return 0;
}

static void fread_graph(struct graph* graph, FILE* file) {
    if (graph == NULL || file == NULL) {
        return;
    }

    size_t vertex_len = 0;
    size_t edge_len = 0; 
    fscanf(file, "%ld %ld", &vertex_len, &edge_len);

    graph_init(graph, true, vertex_len);

    for (size_t i = 0; i < edge_len; i++) {
        vertex_t vi = 0;
        vertex_t wj = 0;
        int32_t weight = 0;
        fscanf(file, "%lu %lu %d", &vi, &wj, &weight);

        graph_addw(graph, vi - 1, wj - 1, weight);
    }
}

static void on_menu(struct graph* graph) {
    bool running = true;
    while (running) {
        printf(" ----------- Graph Menu -----------\n");
        printf(" 1) Show adjacency matrix\n");
        printf(" 2) Connected components\n");
        printf(" 3) Reachable of two vertices\n");
        printf(" 4) Short path of two vertices\n");
        printf(" 5) Short path from one vertex against the other ones\n");
        printf(" 6) Weak path of two vertices\n");
        printf(" 7) Weak path from one vertex against the other ones\n");
        printf(" 8) Salir\n");
        printf("Option: ");

        int option = 0;
        scanf("%d", &option);

        // solo si se cerró la entrada estandar
        if (feof(stdin)) {
            option = 6;
        }

        switch (option) {
            case 1: {
                graph_print(graph);
                break;
            }
            case 2: {
                const struct gcomponent* comp = NULL;
                graph_components(graph, &comp);

                struct hashmap_iterator it = {0};
                hashmap_iterator_init(&it, &comp->map);

                for (struct map_entry entry; hashmap_iterator_next(&it, &entry);) {
                    printf("Connected component %lu (%lu):", it.found_size, entry.key);
                    struct vertex_array* arr = entry.value;

                    for (size_t j = 0; j < arr->len; j++) {
                        printf(" %lu", arr->data[j] + 1);
                    }

                    printf("\n");
                }
                break;

            }
            case 3: {
                vertex_t v = 0;
                vertex_t w = 0;

                printf("Input 1st vertex: ");
                scanf("%lu", &v);
                
                printf("Input 2nd vertex: ");
                scanf("%lu", &w);

                bool reachable = graph_reachable(graph, v - 1, w - 1);
                printf("Is v%lu~v%lu: %s\n", v, w, reachable ? "Yes" : "No");
                break;
            }
            case 4: {
                vertex_t v = 0;
                vertex_t w = 0;

                printf("Input 1st vertex: ");
                scanf("%lu", &v);
                
                printf("Input 2nd vertex: ");
                scanf("%lu", &w);

                u32vertices_map short_paths = {0};
                graph_short_path(graph, v - 1, w - 1, &short_paths);

                struct hashmap_iterator it = {0};
                hashmap_iterator_init(&it, &short_paths);

                printf(" Posible paths:\n");
                for (struct map_entry entry; hashmap_iterator_next(&it, &entry);) {
                    struct vertex_array* vertices = entry.value;

                    printf("  (%lu) ", it.found_size);
                    for (size_t i = 0; i < vertices->len; i++) {
                        printf("%lu", vertices->data[i] + 1);

                        if (i + 1 != vertices->len) {
                            printf("-");
                        }
                    }

                    printf("\n");
                }

                hashmap_destroy(&short_paths);

                break;
            }
            case 5: {
                printf("Input the vertex: ");

                vertex_t v = 0;
                scanf("%lu", &v);

                if (v - 1 >= graph->len) {
                    break;
                }

                struct wave root_wave = {0};
                graph_wave(graph, v - 1, VERTEX_T_MAX, false, &root_wave);

                u32path_map paths = {0};
                wave_to_path(&root_wave, &paths);

                const struct gcomponent* comp = {0};
                graph_components(graph, &comp);
                
                struct vertex_array* members = hashmap_get(&comp->map, comp->array.data[v - 1]);
                size_t max_depth = members != NULL ? members->len : 0;

                for (size_t depth = 2; depth <= max_depth; depth++) {
                    struct hashmap_iterator it = {0};
                    hashmap_iterator_init(&it, &paths);

                    bool is_unlinked = true;

                    for (struct map_entry entry; hashmap_iterator_next(&it, &entry);) {
                        struct path* path = entry.value;
                        struct vertex_array* vertices = &path->vertices;

                        if (vertices->len == depth) {
                            if (is_unlinked) {
                                printf("\n %lu distance of edge:\n  ", depth - 1);
                            } else {
                                printf(", ");
                            }

                            vertex_array_print(vertices);
                            is_unlinked = false;
                        }
                    }
                    
                    if (!is_unlinked) {
                        printf("\n");
                    }
                }

                hashmap_destroy(&paths);
                wave_destroy(&root_wave);
                break;
            }
            case 6: {
                vertex_t v = 0;
                vertex_t w = 0;

                printf("Input 1st vertex: ");
                scanf("%lu", &v);
                
                printf("Input 2nd vertex: ");
                scanf("%lu", &w);

                u32path_map minimal_paths = {0};
                graph_minimal_path(graph, v - 1, w - 1, &minimal_paths);

                struct path* path = hashmap_get(&minimal_paths, w - 1);
                if (path != NULL) {
                    printf(" ");
                    vertex_array_print(&path->vertices);
                    printf(": %d\n", path->weight);
                }

                hashmap_destroy(&minimal_paths);
                break;
            }
            case 7: {
                vertex_t v = 0;

                printf("Input the vertex: ");
                scanf("%lu", &v);
                printf("\n");

                u32path_map minimal_paths = {0};
                graph_minimal_path(graph, v - 1, VERTEX_T_MAX, &minimal_paths);

                struct hashmap_iterator path_it = {0};
                hashmap_iterator_init(&path_it, &minimal_paths);
                for (struct map_entry entry; hashmap_iterator_next(&path_it, &entry);) {
                    struct path* path = entry.value;
                    struct vertex_array* vertices = &path->vertices;

                    printf(" %2lu al %2lu:  ", vertices->data[0] + 1, vertices->data[vertices->len - 1] + 1);
                    vertex_array_print(vertices);
                    printf(": %d\n", path->weight);
                }

                hashmap_destroy(&minimal_paths);
                break;
            }
            case 8: {
                running = false;
                break;
            }
            default: {
                printf("Invalid Option\n");
            }
        }

        if (running) {
            printf("\nPress enter to continue");

            int key = 0;
            for (int lines = 0; key != EOF && lines < 2;) {
                key = fgetc(stdin);

                if (key == '\r' || key == '\n') {
                    lines++;
                }
            }
        }
    }
}

