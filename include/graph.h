#ifndef ED_GRAPH_GUARD_HEADER
#define ED_GRAPH_GUARD_HEADER

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "list.h"
#include "map.h"

#include "vertex.h"
#include "wave.h"
#include "path.h"

/**
 * Representa el valor en el que en no existe un vinculo de
 * dos vertices.
 */
#define NONE_WEIGHT32_VALUE INT32_MAX

/**
 * Representa las diferentes componentes conexas de un grafo.
 *
 * @see gcomponent_destroy
 *
 * @member array es una estructura en el cual vincula el indice
 *               con un vertice (vertex_t) en el rango [0, len)
 *               con la ID de la componente conexa en el que
 *               pertenence 
 * @member map es donde se enlaza la ID de la componente conexa
 *         con una lista de vertices (vertex_t) que pertenenen
 *         a la misma componente conexa
 */
struct gcomponent {
    struct {
        size_t len;
        uint32_t* data;
    } array;

    u32vertices_map map;
};

/** 
 * La representación de un grafo indurecto en una matriz de
 * adyacencia de 32bits.
 * 
 * @see graph_init
 * @see graph_destroy
 *
 * @member weighted indica si el grafo es pesado
 * @member cache es usado internamente para acelerar algunas
 *               operaciones
 * @member len la cantidad de vertices que hay en el grafo
 * @member matrix almacena los vinculos entre dos vertices;
 *                si es un grafo pesado entonces se guardará
 *                el peso de dicho vinculo
 */
struct graph {
    bool weighted;

    struct {
        struct gcomponent* component;
    } cache;

    size_t len;
    int32_t** matrix;
};

/**
 * Inicializa un grafo para ser usado.
 *
 * @see graph_destroy
 *
 * @param graph el grafo a inicializar
 * @param weighted si el grafo es pesado
 * @param len la cantidad de vertices que habrá en el grafo
 */
void graph_init(struct graph* graph, bool weighted, size_t len);
/**
 * Imprime en el estandar de salida la matriz de adyacencia
 * del grafo.
 *
 * @param graph el grafo a ser imprimido
 */
void graph_print(const struct graph* graph);
/**
 * Destruye un grafo ya inicializado.
 *
 * @param graph el grafo a destruir
 */
void graph_destroy(struct graph* graph);

/**
 * Vincula dos vertices en el grafo con un peso.
 *
 * Si los vertices están fueran de los bordes del grafo, no se
 * aplicará ningun vinculo.
 *
 * @param graph el grafo a vincular los vertices
 * @param vi el vertice origen (a partir de)
 * @param wj el vertice destino (hasta a)
 * @param weight el peso del vinculo, si el grafo no es pesado
 *               entonces se tomara en cuenta valores de 0 y 1
 */
void graph_addw(struct graph* graph, vertex_t vi, vertex_t wj, int32_t weight);
/**
 * Vincula dos vertices en el grafo.
 *
 * @see graph_addw
 *
 * @param graph el grafo a vincular los vertices
 * @param vi el vertice origen (a partir de)
 * @param wj el vertice destino (hasta a)
 */
void graph_add(struct graph* graph, vertex_t vi, vertex_t wj);
/**
 * Comprueba que dos vertices están vinculados en el grafo.
 *
 * @param graph el grafo a verificar los vertices
 * @param vi el vertice origen (a partir de)
 * @param wj el vertice destino (hasta a)
 */
bool graph_has(const struct graph* graph, vertex_t vi, vertex_t wj);
/**
 * Devuelve el peso de un vinculo entre dos vertices del grafo.
 *
 * Si el vinculo no existe entonces no se aplicará ninguna
 * acción.
 *
 * @see graph_has
 *
 * @param graph el grafo a buscar el peso del vinculo
 * @param vi el vertice origen (a partir de)
 * @param wj el vertice destino (hasta a)
 * @return donde se guardará el peso del vinculo
 */
int32_t graph_get(const struct graph* graph, vertex_t vi, vertex_t wj);
/**
 * Deshace el vinculo de dos vertices del grafo.
 *
 * @param graph el grafo a deshacer el vinculo
 * @param vi el vertice origen (a partir de)
 * @param wj el vertice destino (hasta a)
 */
void graph_del(struct graph* graph, vertex_t vi, vertex_t wj);

/**
 * Cuenta la cantidad de vinculos que posee vi del grafo.
 *
 * La cantidad va de acuerdo a (matemáticamente):
 *     G = (V, A)
 *     (vi, w) para todo w en V
 *
 * @param graph el grafo a contar los vinculos de vi
 * @param vi el vertice origen (a partir de)
 */
size_t graph_rcount(const struct graph* graph, vertex_t vi); 
/**
 * Cuenta la cantidad de vinculos que posee wj del grafo.
 *
 * La cantidad va de acuerdo a (matemáticamente):
 *     G = (V, A)
 *     (v, wj) para todo v en V
 *
 * @param graph el grafo a contar los vinculos de wj
 * @param wj el vertice destino (hasta a)
 */
size_t graph_ccount(const struct graph* graph, vertex_t wj); 

/**
 * Genera las ondas desde un vertice de origen hasta un posible
 * vertice final.
 *
 * @param graph el grafo donde se generarán las ondas
 * @param start_vertex el vertice de origen
 * @param end_vertex el vertice final, VERTEX_T_MAX si ninguno
 * @param should_duplicate si existen vertices en el que están
 *                         conectados entre si.
 * @param out_wave las ondas generadas
 */
void graph_wave(const struct graph* graph,
                vertex_t start_vertex,
                vertex_t end_vertex,
                bool should_duplicate,
                struct wave* out_wave);
/**
 * Evalua las componentes conexas que existen en el grafo.
 *
 * @param graph el grafo a detectar las componentes conexas
 * @param out_comp donde se guardará las componentes conexas
 *                 pero en modo solo-lectura, out_comp puede
 *                 ser NULL
 */
void graph_components(struct graph* graph, const struct gcomponent** out_comp);
/**
 * Verifica si dos vertices son alcanzable en el grafo.
 *
 * Dos vertices son alcanzable solo si existe un camino, de la
 * misma forma, existe un camino si ambos vertices pertenencen
 * a la misma componente conexa.
 *
 * @see graph_components
 *
 * @param graph el grafo en el que pertenencen los vertices
 * @param start_vertex el vertice a partir
 * @param end_vertex el vertice a llegar
 * @return true si es alcanzable de lo contrario false
 */
bool graph_reachable(struct graph* graph, vertex_t start_vertex, vertex_t end_vertex);
/**
 * Encuentra las rutas más cortas entre dos vertices en el
 * grafo.
 *
 * Se tomará en cuenta lo siguiente:
 *   1. Ambos vertices deben ser alcanzable, de otra forma,
 *      no podrá existir un camino que una ambos vertices.
 *   2. Se define el camino más corto; como el menor número
 *      de vinculos (arcos).
 *   3. Pueden existir varios caminos en el que son
 *      equivalentemente cortos, por lo que serán incluidos.
 *
 * @param graph el grafo para evaluar el camino más corto
 * @param start_vertex el vertice de partida
 * @param end_vertex el vertice de destino
 * @param out_map donde se guardarán los caminos encontrados
 */
void graph_short_path(struct graph* graph,
                      vertex_t start_vertex,
                      vertex_t end_vertex,
                      u32vertices_map* out_map);
/**
 * Encuentra las rutas más débil entre dos vertices en el
 * grafo.
 *
 * Se tomará en cuenta lo siguiente:
 *   1. Ambos vertices deben ser alcanzable, de otra forma,
 *      no podrá existir un camino que una ambos vertices.
 *   2. Se define el camino más débil; como la sumatoria
 *      del peso de los vinculos (arcos) la más minima posible.
 *   3. Pueden existir varios caminos en el que son
 *      equivalentemente débil, por lo que serán incluidos.
 *
 * @param graph el grafo para evaluar el camino más débil
 * @param start_vertex el vertice de partida
 * @param end_vertex el vertice de destino
 * @param out_map donde se guardarán los caminos encontrados
 */
void graph_minimal_path(struct graph* graph,
                        vertex_t start_vertex,
                        vertex_t end_vertex,
                        u32path_map* out_map);

/**
 * Destruye un componente ya inicializado.
 *
 * @param comp el componente a destruir
 */
void gcomponent_destroy(struct gcomponent* comp);

#endif // ED_GRAPH_GUARD_HEADER
