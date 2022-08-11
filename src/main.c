#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <graph.h>

/**
 * Lee desde un archivo los datos de un grafo.
 *
 * @param graph el grafo a almcenar los datos
 * @param file el archivo a leer
 */
static void fread_graph(struct graph* graph, FILE* file);
/**
 * El menu principal.
 *
 * @param graph el grafo
 */
static void on_menu(struct graph* graph);

int main(int argc, char** args) {
    const char* filename = argc > 1 ? args[1] : "Panas.in";
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("El archivo %s no existe o no puede ser leido\n", filename);
        return 1;
    }

    struct graph graph = {};

    printf("Cargando archivo: %s\n", filename);
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
        printf(" ----------- Menu Grafo -----------\n");
        printf(" 1) Ver matriz de adyacencia\n");
        printf(" 2) Componentes conexas\n");
        printf(" 3) Alcanzabilidad de dos vertices\n");
        printf(" 4) Camino corto de dos vertices\n");
        printf(" 5) Camino debil de dos vertices\n");
        printf(" 6) Salir\n");
        printf("Opción: ");

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

                struct hashmap_iterator it = {};
                hashmap_iterator_init(&it, &comp->map);

                for (struct map_entry entry; hashmap_iterator_next(&it, &entry);) {
                    printf("Aldea %lu (%lu):", it.found_size, entry.key);
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

                printf(" Ingrese 1er vertice: ");
                scanf("%lu", &v);
                
                printf(" Ingrese 2do vertice: ");
                scanf("%lu", &w);

                bool reachable = graph_reachable(graph, v - 1, w - 1);
                printf("Es v%lu~v%lu: %s\n", v, w, reachable ? "Si" : "No");
                break;
            }
            case 4: {
                vertex_t v = 0;
                vertex_t w = 0;

                printf(" Ingrese 1er vertice: ");
                scanf("%lu", &v);
                
                printf(" Ingrese 2do vertice: ");
                scanf("%lu", &w);

                u32vertices_map short_paths = {};
                graph_short_path(graph, v - 1, w - 1, &short_paths);

                struct hashmap_iterator it = {};
                hashmap_iterator_init(&it, &short_paths);

                printf(" Posibles caminos:\n");
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
                break;
            }
            case 6: {
                running = false;
                break;
            }
            default: {
                printf("Opción invalida\n");
            }
        }

        if (running) {
            printf("\nPulse enter para continuar");

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

