#ifndef ED_VERTEX_GUARD_HEADER
#define ED_VERTEX_GUARD_HEADER

#include <stddef.h>
#include <stdlib.h>

#include "map.h"
#include "list.h"

#define VERTEX_T_MAX SIZE_MAX

/**
 * Represents a vertex.
 */
typedef size_t vertex_t;

/**
 * Represents a HashMap that links uint32_t (the key) with a
 * struct vertex_array (the value).
 *
 * Seen in another way: HashMap<uint32_t, struct vertex_array>
 *
 * @see hashmap
 * @see u32vertices_destroyer
 */
typedef struct hashmap u32vertices_map;

/**
 * Represents a queue that stores vertex_t.
 *
 * @member internal the queue used internally
 */
struct queue_vertex {
    queue_t internal;
};

/**
 * Represents a vertex sequence.
 * 
 * @see vertex_array_reserve
 * @see vertex_array_destroy
 *
 * @param capacity the sequence capacity
 * @param len the sequence size
 * @param data is where vertices are stored
 */
struct vertex_array {
    size_t capacity;
    size_t len;

    vertex_t* data;
};

/**
 * Create a vertex sequence from an array.
 *
 * @param array the sequence to store the data
 * @param data the array to copy
 * @param len the array length
 */
void vertex_array_from(struct vertex_array* array, const vertex_t* data, size_t len);
/**
 * Reserve more space to vertex sequence.
 *
 * If capacity is 0, then it won't apply any action.
 *
 * @param array the sequence to reserve space
 * @param capacity the capacity to reserve
 */
void vertex_array_reserve(struct vertex_array* array, size_t capacity);
/**
 * Clone a vertex sequence into another one.
 *
 * @param array the sequence to clone
 * @param out the cloned sequence
 */
void vertex_array_clone(const struct vertex_array* array, struct vertex_array* out);
/**
 * Print a vertex sequence.
 *
 * @param array the sequence to print
 */
void vertex_array_print(struct vertex_array* array);
/**
 * Destroy a vertex sequence.
 *
 * @param array the sequence to destroy
 */
void vertex_array_destroy(struct vertex_array* array);

/**
 * Initialize a queue of vertex_t.
 * 
 * @param queue tthe queue to initialize
 */
static inline void queue_vertex_init(struct queue_vertex* queue) {
    queue_init(&queue->internal, free);
}

/**
 * Check if queue is not empty.
 *
 * @param queue the queue to check
 * @return true if it's empty, otherwise false
 */
static inline bool queue_vertex_empty(struct queue_vertex* queue) {
    return queue_empty(&queue->internal);
}

/**
 * Return the actual size of queue.
 *
 * @param queue the queue to know the size
 * @return the queue size
 */
static inline size_t queue_vertex_size(struct queue_vertex* queue) {
    return queue->internal.size;
}

/**
 * Destroy an initialized queue.
 *
 * @param queue the queue to destroy
 */
static inline void queue_vertex_destroy(struct queue_vertex* queue) {
    queue_destroy(&queue->internal);
}

/**
 * Add a vertex into the queue.
 *
 * @param queue the queue where to add the vertex in
 * @param vertex the vertex to add
 */
static inline void queue_vertex_add(struct queue_vertex* queue, vertex_t vertex) {
    vertex_t* vertex_ptr = malloc(sizeof(vertex_t));
    *vertex_ptr = vertex;

    queue_add(&queue->internal, vertex_ptr);
}

/**
 * Extract a vertex from queue.
 *
 * @param queue the queue where to extract the vertex from
 * @return the vertex, VERTEX_T_MAX if the queue is empty
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
 * The value destructor of u32vertices_map.
 *
 * @see hashmap#destroyer
 */
void u32vertices_destroyer(void* array);

#endif // ED_VERTEX_GUARD_HEADER
