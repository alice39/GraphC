#ifndef ED_STACK_GUARD_HEADER
#define ED_STACK_GUARD_HEADER

#include <stdbool.h>
#include <stddef.h>

/**
 * Represents the value destructor of a linked list.
 */
typedef void (*lfree_t)(void*);

/**
 * The node of a linked list.
 *
 * @member data the stored value
 * @member next the next node, NULL if there is no
 * @member back the back node, NULL if there is no
 */
struct list_node {
    void* data;

    struct list_node* next;
    struct list_node* back;
};

/**
 * The data structure that represents a Linked List.
 *
 * @see list_init
 * @see list_destroy
 *
 * @member destroyer the function that destroyes the stored
 *                   value in the list, it can be NULL if it is
 *                   not needed
 * @member size the size of list
 * @member head the first node in the list, NULL if there is no
 * @member last the last node in the list, NULL if there is no
 */
struct list {
    lfree_t destroyer;

    size_t size;
    struct list_node* head;
    struct list_node* last;
};

/**
 * The data type that represents a Queue.
 *
 * @see struct list
 */
typedef struct list queue_t;
/**
 * The data type that representa a Stack.
 *
 * @see struct list
 */
typedef struct list stack_t;

/**
 * Initialize a linked list.
 *
 * @param list the list to initialize
 * @param destroyer value destructor, NULL if there is no
 */
void list_init(struct list* list, lfree_t destroyer);
/**
 * Check if the list is empty.
 *
 * @param list the list to check
 * @return true if it's empty, otherwise false
 */
bool list_empty(struct list* list);
/**
 * Destroy an initialized list.
 *
 * @param list the list to destroy
 */
void list_destroy(struct list* list);

/**
 * Add an element at the list's beginning.
 *
 * @param list the list where to add the element
 * @param data the element to add
 */
void list_add_head(struct list* list, void* data);
/**
 * Add an element at the list's ending.
 *
 * @param list the list where to add the element
 * @param data the element to add
 */
void list_add_last(struct list* list, void* data);

/**
 * Delete the head list's element.
 *
 * @param list the list where to delete the element
 * @return the stored value
 */
void* list_del_head(struct list* list);
/**
 * Delete the last list's element.
 *
 * @param list the list where to delete the element
 * @return the stored value
 */
void* list_del_last(struct list* list);

/**
 * Initialize a queue.
 *
 * @param queue the queue to initialize
 * @param destroyer the value destructor, it can be NULL
 */
static inline void queue_init(queue_t* queue, lfree_t destroyer) {
    list_init(queue, destroyer);
}

/**
 * Check if the queue is empty.
 *
 * @return true if it's empty, otherwise false
 */
static inline bool queue_empty(queue_t* queue) {
    return list_empty(queue);
}

/**
 * Destroy an initialized queue.
 *
 * @param queue the queue to destroy
 */
static inline void queue_destroy(queue_t* queue) {
    list_destroy(queue);
}

/**
 * Add an element in the queue.
 *
 * @param queue the queue where to add the element to
 * @param data the element to be added
 */
static inline void queue_add(queue_t* queue, void* data) {
    list_add_last(queue, data);
}

/**
 * Extract the next queue's element.
 *
 * @param queue the queue to extract the element from
 * @return the element that had
 */
static inline void* queue_del(queue_t* queue) {
    return list_del_head(queue);
}

/**
 * Initialize a stack.
 *
 * @param stack the stack to initilize
 * @param destroyer the value destructor, it can be NULL
 */
static inline void stack_init(stack_t* stack, lfree_t destroyer) {
    list_init(stack, destroyer);
}

/**
 * Check if the stack is empty.
 *
 * @param stack the stack to check
 * @return true if it's empty, otherwise false
 */
static inline bool stack_empty(stack_t* stack) {
    return list_empty(stack);
}

/**
 * Destroy an initialized stack.
 *
 * @param stack the stack to destroy
 */
static inline void stack_destroy(stack_t* stack) {
    list_destroy(stack);
}

/**
 * Push an element at the stack's top.
 *
 * @param stack the stack where to add the element to
 * @param data the element to push
 */
static inline void stack_push(stack_t* stack, void* data) {
    list_add_last(stack, data);
}

/**
 * Pop an element from stack's top.
 *
 * @param stack the stack where to pop the element from
 * @return the element that had
 */
static inline void* stack_pop(stack_t* stack) {
    return list_del_last(stack);
}

#endif // ED_STACK_GUARD_HEADER
