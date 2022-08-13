#include <stdlib.h>
#include <string.h>

#include <vertex.h>

void vertex_array_from(struct vertex_array* array, const vertex_t* data, size_t len) {
    if (array == NULL || data == NULL) {
        return;
    }

    // reserva el espacio suficiente si es necesario
    if (len > array->capacity) {
        vertex_array_reserve(array, len - array->capacity);
    }

    array->len = len;
    memcpy(array->data, data, sizeof(vertex_t) * len);
}

void vertex_array_reserve(struct vertex_array* array, size_t capacity) {
    if (array == NULL || array->capacity > array->len + capacity) {
        return;
    }

    size_t new_cap = array->capacity + capacity;
    vertex_t* new_data = realloc(array->data, sizeof(vertex_t) * new_cap);

    // es posible que la nueva capacidad no pueda
    // ser reservada por falta de memoria
    if (new_data == NULL) {
        return;
    }

    array->capacity = new_cap;
    array->data = new_data;
}

void vertex_array_clone(const struct vertex_array* array, struct vertex_array* out) {
    if (array == NULL || out == NULL) {
        return;
    }

    vertex_array_destroy(out);
    vertex_array_reserve(out, array->len);

    out->len = array->len;
    memcpy(out->data, array->data, sizeof(vertex_t) * array->len);
}

void vertex_array_backwards(struct vertex_array* array) {
    if (array == NULL) {
        return;
    }

    for (size_t i = 0; i < array->len / 2; i++) {
        size_t mirror_i = array->len - i - 1;

        vertex_t swap = array->data[i];
        array->data[i] = array->data[mirror_i];
        array->data[mirror_i] = swap;
    }
}

void vertex_array_destroy(struct vertex_array* array) {
    if (array == NULL) {
        return;
    }

    array->capacity = 0;
    array->len = 0;
    free(array->data);
    array->data = NULL;
}

void u32vertices_destroyer(void* array) {
    vertex_array_destroy(array);
    free(array);
}

