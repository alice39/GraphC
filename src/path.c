#include "path.h"

void path_zinit(struct path* path) {
    if (path == NULL) {
        return;
    }

    vertex_array_destroy(&path->vertices);
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

