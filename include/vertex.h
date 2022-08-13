#ifndef ED_VERTEX_GUARD_HEADER
#define ED_VERTEX_GUARD_HEADER

#include <stddef.h>
#include <stdlib.h>

#include "map.h"
#include "list.h"

#define VERTEX_T_MAX SIZE_MAX

/**
 * Representa un vertice.
 */
typedef size_t vertex_t;

/**
 * Representa un HashMap en el que vincula uint32_t (la llave)
 * con un struct vertex_array (el valor).
 *
 * Visto de otra forma: HashMap<uint32_t, struct vertex_array>
 *
 * @see hashmap
 * @see u32vertices_destroyer
 */
typedef struct hashmap u32vertices_map;

/**
 * Representa una cola que almacena vertex_t.
 *
 * @member internal la cola usada internamente
 */
struct queue_vertex {
    queue_t internal;
};

/**
 * Representa una secuencia/lista de vertices.
 * 
 * @see vertex_array_reserve
 * @see vertex_array_destroy
 *
 * @param len el tamaño de la secuencia
 * @param data donde se guardan los vertices
 */
struct vertex_array {
    size_t len;
    vertex_t* data;
};

/**
 * Crea una secuencia de vertices desde un arreglo.
 *
 * @param array la secuencia a almacenar los datos
 * @param data el arreglo para copiar
 * @param len la longitud del arreglo
 */
void vertex_array_from(struct vertex_array* array, const vertex_t* data, size_t len);
/**
 * Reserva más espacio a la secuencia de vertices.
 *
 * Si capacity es 0, no se aplicará ninguna acción.
 *
 * @param array la secuencia donde se reervará espacio
 * @param capacity la cantidad de espacio a reservar
 */
void vertex_array_reserve(struct vertex_array* array, size_t capacity);
/**
 * Clona los datos de una secuencia a otra.
 *
 * @param array la secuencia a clonar
 * @param out la secuencia de salida clonada
 */
void vertex_array_clone(const struct vertex_array* array, struct vertex_array* out);
/**
 * Coloca los elementos de la secuencia en orden reverso.
 *
 * @param array la secuencia a modidificar el orden
 */
void vertex_array_backwards(struct vertex_array* array);
/**
 * Elimina toda la información de la secuencia.
 *
 * @param array la secuencia a destruir
 */
void vertex_array_destroy(struct vertex_array* array);

/**
 * Inicializa una cola de vertex_t.
 * 
 * @param queue la cola a inicializar
 */
static inline void queue_vertex_init(struct queue_vertex* queue) {
    queue_init(&queue->internal, free);
}

/**
 * Verifica si la cola no está vacia.
 *
 * @param queue la cola a verificar
 * @return si no está vacia
 */
static inline bool queue_vertex_empty(struct queue_vertex* queue) {
    return queue_empty(&queue->internal);
}

/**
 * Regresa el tamaño actual de la cola.
 *
 * @param queue la cola a saber el tamaño
 * @return el tamaño de la cola
 */
static inline size_t queue_vertex_size(struct queue_vertex* queue) {
    return queue->internal.size;
}

/**
 * Destruye ya una cola inicializada.
 *
 * @param queue la cola a destruir
 */
static inline void queue_vertex_destroy(struct queue_vertex* queue) {
    queue_destroy(&queue->internal);
}

/**
 * Añade un vertice a la cola.
 *
 * @param queue la cola a añadir el vertice
 * @param vertex el vertice a añadir
 */
static inline void queue_vertex_add(struct queue_vertex* queue, vertex_t vertex) {
    vertex_t* vertex_ptr = malloc(sizeof(vertex_t));
    *vertex_ptr = vertex;

    queue_add(&queue->internal, vertex_ptr);
}

/**
 * Extrae un vertice de la cola.
 *
 * @param queue la cola a extraer el vertice
 * @return el vertice, VERTEX_T_MAX si la cola está vacia
 */
static inline vertex_t queue_vertex_del(struct queue_vertex* queue) {
    vertex_t* vertex_ptr = queue_del(&queue->internal);
    if (vertex_ptr == NULL) {
        return VERTEX_T_MAX;
    }

    vertex_t vertex = *vertex_ptr;
    free(vertex_ptr);
    return vertex;
}

/**
 * El destructor del hashmap de u32vertices_map.
 *
 * @see hashmap#destroyer
 */
void u32vertices_destroyer(void* array);

#endif // ED_VERTEX_GUARD_HEADER
