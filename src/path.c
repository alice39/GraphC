#include <string.h>

#include "path.h"

void path_init(struct path* path, struct vertex_array* vertices) {
    if (path == NULL) {
        return;
    }

    if (vertices != NULL) {
        memcpy(&path->vertices, vertices, sizeof(struct vertex_array));
    } else {
        vertex_array_destroy(&path->vertices);
    }

    path->weight = 0;
}

void path_destroy(struct path* path) {
    if (path == NULL) {
        return;
    }

    vertex_array_destroy(&path->vertices);
    path->weight = 0;
}

void u32path_destroyer(void* path) {
    path_destroy(path);
    free(path);
}

