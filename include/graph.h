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
 * Represents the value that indicates that there is no edge
 * between two vertices.
 */
#define NONE_WEIGHT32_VALUE INT32_MAX

/**
 * Represents the different connected components of a graph.
 *
 * @see gcomponent_destroy
 *
 * @member array is a structure which links the index of a
 *               vertex (vertex_t) in the interval [0, len)
 *               with the ID of a connected component where
 *               it belongs to
 * @member map is where links the connected component's ID
 *         with an array of vertices (vertex_t) that belong to
 *         the same connected component
 */
struct gcomponent {
    struct {
        size_t len;
        uint32_t* data;
    } array;

    u32vertices_map map;
};

/** 
 * Represents an undirected graph in an adjacency matrix of
 * 32bits.
 * 
 * @see graph_init
 * @see graph_destroy
 *
 * @member weighted indicates if the graph is weighted
 * @member cache is used internally to speed up some operations
 * @member len is the length of vertices that there are in
 * @member matrix stores the edges between two vertices;
 *                if the graph is weighted then it'll store
 *                the given weight otherwise 1
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
 * Initialize a graph to be used for.
 *
 * @see graph_destroy
 *
 * @param graph the graph to initialize
 * @param weighted if the graph is weighted
 * @param len the length of vertices that there will be
 */
void graph_init(struct graph* graph, bool weighted, size_t len);
/**
 * Print in STDOUT the adjacency matrix of given graph.
 *
 * @param graph the graph to be printed
 */
void graph_print(const struct graph* graph);
/**
 * Destroy a initialized graph.
 *
 * @param graph the graph to destroy
 */
void graph_destroy(struct graph* graph);

/**
 * Add an edge between two vertices in the graph with a weight.
 *
 * If the vertices are out of range (in graph size), it will
 * not apply any edge.
 *
 * @param graph the graph to add the edge
 * @param vi the source vertex
 * @param wj the destination vertex
 * @param weight the edge's weight, if the graph is not
 *               weighted then it'll take in count 1 values
 */
void graph_addw(struct graph* graph, vertex_t vi, vertex_t wj, int32_t weight);
/**
 * Add an edge between two vertices in the graph.
 *
 * @see graph_addw
 *
 * @param graph the graph to add the edge
 * @param vi the source vertex
 * @param wj the destination vertex
 */
void graph_add(struct graph* graph, vertex_t vi, vertex_t wj);
/**
 * Check if two vertices have an edge in the graph.
 *
 * @param graph the graph to check the vertices
 * @param vi the source vertex
 * @param wj the destination vertex
 * @return true if they have an edge, false otherwise
 */
bool graph_has(const struct graph* graph, vertex_t vi, vertex_t wj);
/**
 * Return the weight of two vertices's edge in the graph.
 *
 * If there's no edge, then it'll not apply any action.
 *
 * @see graph_has
 *
 * @param graph the graph to look for edge's weight
 * @param vi the source vertex
 * @param wj the destination vertex
 * @return the edge's weighto or NONE_WEIGHT32_VALUE if
 *         there is no
 */
int32_t graph_get(const struct graph* graph, vertex_t vi, vertex_t wj);
/**
 * Undo the vertices's edge in the graph.
 *
 * @param graph the graph to undo the edge
 * @param vi the source vertex
 * @param wj the destination vertex
 */
void graph_del(struct graph* graph, vertex_t vi, vertex_t wj);

/**
 * Count how many edges a vertice vi in the graph.
 *
 * The size goes according to (mathetimatically):
 *     G = (V, A)
 *     (vi, w) for all w in V
 *
 * @param graph the graph to count the vi's edges
 * @param vi the source vertex
 * @return the size of edges that vertice vi has
 */
size_t graph_rcount(const struct graph* graph, vertex_t vi); 
/**
 * Count how many edges a vertice wj in the graph.
 *
 * The size goes according to (mathetimatically):
 *     G = (V, A)
 *     (v, wj) for all v in V
 *
 * @param graph the graph to count the wj's edges
 * @param wj the source vertex
 * @return the size of edges that vertice wj has
 */
size_t graph_ccount(const struct graph* graph, vertex_t wj); 

/**
 * Generate the waves from a start vertex until a possible end
 * vertex.
 *
 * @param graph the graph where to generate the waves on
 * @param start_vertex the source vertex
 * @param end_vertex the destination vertex, VERTEX_T_MAX if
 *                   none
 * @param should_duplicate if there are vertices that are
 *                         inter-connected between themselves
 * @param out_wave the generated waves
 */
void graph_wave(const struct graph* graph,
                vertex_t start_vertex,
                vertex_t end_vertex,
                bool should_duplicate,
                struct wave* out_wave);
/**
 * Evalue the connected components that exist in the graph.
 *
 * @param graph the graph to detect the connected components
 * @param out_comp where it'll store the connected components
 *                 but in read-only mode, out_comp can be NULL
 */
void graph_components(struct graph* graph, const struct gcomponent** out_comp);
/**
 * Check if two vertices are reachable in the graph.
 *
 * Two vertices are reachable just if it exists a path, in
 * the same way, it exists a pth just if both vertices belong
 * to the same connected component.
 *
 * @see graph_components
 *
 * @param graph the graph that vertices belong in
 * @param start_vertex the source vertex
 * @param end_vertex the destionation vertex
 * @return true if reachable, otherwise false
 */
bool graph_reachable(struct graph* graph, vertex_t start_vertex, vertex_t end_vertex);
/**
 * Find the shortest paths between two vertices in the graph.
 *
 * It'll take in count the followings:
 *   1. Both vertices must be reachable, otherwise, it'll not
 *      be able to exist a path that connects both vertices.
 *   2. A short path is defined as; the lower size of edges.
 *   3. It can exist several paths that are equivally shorts,
 *      thereby it will be included too.
 *
 * @param graph the graph to evalue the shortest path
 * @param start_vertex the source vertex
 * @param end_vertex the destination vertex
 * @param out_map where it'll be stored the paths found
 */
void graph_short_path(struct graph* graph,
                      vertex_t start_vertex,
                      vertex_t end_vertex,
                      u32vertices_map* out_map);
/**
 * Find the weakest paths between two vertices in the graph.
 *
 * It'll take in count the followings:
 *   1. Both vertices must be reachable, otherwise, it'll not
 *      be able to exist a path that connects both vertices.
 *   2. A weak path is defined as; the lower sum of weights.
 *   3. It'll store all weakest paths found until to reach
 *      the destination vertex, all of them if VERTEX_T_MAX is
 *      present.
 *
 * @param graph the graph to evalue the shortest path
 * @param start_vertex the source vertex
 * @param end_vertex the destination vertex
 * @param out_map where it'll be stored the paths found
 */
void graph_minimal_path(struct graph* graph,
                        vertex_t start_vertex,
                        vertex_t end_vertex,
                        u32path_map* out_map);

/**
 * Destroy an initialized component.
 *
 * @param comp the component to destroy
 */
void gcomponent_destroy(struct gcomponent* comp);

#endif // ED_GRAPH_GUARD_HEADER
