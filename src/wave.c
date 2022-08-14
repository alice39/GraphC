#include <stdio.h>
#include <stdlib.h>

#include <wave.h>

void wave_init(struct wave* wave, struct wave* parent, vertex_t vertex) {
    if (wave == NULL) {
        return;
    }

    wave->parent = parent;
    wave->depth = parent != NULL ? parent->depth + 1 : 0; 
    wave->vertex = vertex;
    list_init(&wave->subwaves, wave_destroyer);
}

void wave_print(struct wave* wave) {
    if (wave == NULL) {
        return;
    }

    stack_t wave_stack = {0};
    stack_init(&wave_stack, NULL);
    stack_push(&wave_stack, wave);

    while (!stack_empty(&wave_stack)) {
        struct wave* pop_wave = stack_pop(&wave_stack);

        for (size_t i = 0; i < pop_wave->depth; i++) {
            printf(" ");
        }
        printf("%lu\n", pop_wave->vertex + 1);

        struct list_node* node = pop_wave->subwaves.last;
        for (; node != NULL; node = node->back) {
            stack_push(&wave_stack, node->data);
        }
    }

    stack_destroy(&wave_stack);
}

struct wave* wave_add(struct wave* wave, vertex_t vertex) {
    if (wave == NULL) {
        return NULL;
    }

    // verifica si ya hay uno existente para evitar
    // duplicados
    struct wave* found = wave_get(wave, vertex);
    if (found != NULL) {
        return found;
    }

    struct wave* next_wave = calloc(1, sizeof(struct wave));
    wave_init(next_wave, wave, vertex);

    list_add_last(&wave->subwaves, next_wave);

    return next_wave;
}

struct wave* wave_get(struct wave* wave, vertex_t vertex) {
    if (wave == NULL) {
        return NULL;
    }

    struct list_node* node = wave->subwaves.head;
    for (; node != NULL; node = node->next) {
        struct wave* value = node->data;
        if (value->vertex == vertex) {
            return value;
        }
    }

    return NULL;
}

void wave_to_path(struct wave* wave, u32path_map* out_map) {
    if (wave == NULL || out_map == NULL) {
        return;
    }

    hashmap_init(out_map, 0, u32path_destroyer);
    mkey_t next_key = 0;

    stack_t stack = {0};
    stack_init(&stack, NULL);
    stack_push(&stack, wave);

    size_t root_depth = wave->depth;
    // la secuencia de modelo actual para ser copiada
    struct vertex_array model = {0};

    while (!stack_empty(&stack)) {
        struct wave* pop_wave = stack_pop(&stack);

        // se añade el vertice de la onda actual en el modelo
        vertex_array_reserve(&model, 1);
        model.data[pop_wave->depth - root_depth] = pop_wave->vertex;
        model.len = pop_wave->depth - root_depth + 1;

        // se hace una copia del model para añadirla a los
        // caminos generados en out_map

        struct vertex_array vertices = {0};
        vertex_array_clone(&model, &vertices);

        struct path* path = calloc(1, sizeof(struct path));
        path_init(path, &vertices);
        hashmap_put(out_map, next_key++, path);

        // se añaden todas las ondas subyacentes a esta onda
        // para seguir el mismo modelo

        struct list_node* node = pop_wave->subwaves.last;
        for (; node != NULL; node = node->back) {
            stack_push(&stack, node->data);
        }
    }

    vertex_array_destroy(&model);

    // quitamos el camino raiz o el camino con 1 vinculo
    u32path_destroyer(hashmap_del(out_map, 0));
}

void wave_destroy(struct wave* wave) {
    if (wave == NULL) {
        return;
    }

    wave->parent = NULL;
    wave->depth = 0;
    wave->vertex = 0;
    list_destroy(&wave->subwaves);
}

void wave_destroyer(void* wave) {
    wave_destroy(wave);
    free(wave);
}

