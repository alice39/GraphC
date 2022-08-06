#ifndef ED_REPGRAPH_GUARD_HEADER
#define ED_REPGRAPH_GUARD_HEADER

#include "graph.h"
#include "vertex.h"

struct rep_graph {
    struct graph internal;
    u32vertices_map mapping;
};

static inline void rgraph_init(struct rep_graph* graph, bool undirected, bool wieghted, u32vertices_map vertices) {
    graph_init(&graph->internal, undirected, wieghted, hashmap_size(&vertices));
    graph->mapping = vertices;
}

static inline void rgraph_print(struct rep_graph* graph) {
    graph_print(&graph->internal);
}

static inline void rgraph_destroy(struct rep_graph* graph) {
    graph_destroy(&graph->internal);
    hashmap_destroy(&graph->mapping);
}

#endif // ED_REPGRAPH_GUARD_HEADER
