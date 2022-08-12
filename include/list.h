#ifndef ED_STACK_GUARD_HEADER
#define ED_STACK_GUARD_HEADER

#include <stdbool.h>
#include <stddef.h>

/**
 * Representa el destructor de valores de la lista enlazada.
 */
typedef void (*lfree_t)(void*);

/**
 * El nodo de la lista enlazada.
 *
 * @member data el valor almacenado
 * @member next el nodo siguiente, NULL si
 *         no hay
 * @member back el nodo anterior, NULL si
 *         no hay
 */
struct list_node {
    void* data;

    struct list_node* next;
    struct list_node* back;
};

/**
 * La estructura de datos que representa una Lista Enlazada.
 *
 * @see list_init
 * @see list_destroy
 *
 * @member destroyer la función que destruye el valor
 *                   almacenado en el mapa, puede ser NULL
 *                   si no es necesario
 * @member head el primer nodo de la lista, NULL si no hay
 * @member last el último nodo de la lista, NULL si no hay
 */
struct list {
    lfree_t destroyer;

    struct list_node* head;
    struct list_node* last;
};

/**
 * El tipo de dato que representa una Cola.
 *
 * @see struct list
 */
typedef struct list queue_t;
/**
 * El tipo de dato que representa una Pila.
 *
 * @see struct list
 */
typedef struct list stack_t;

/**
 * Inicializa una lita enlazada.
 *
 * @param list la lista a inicializar
 * @param destroyer destructor de valores, puede ser NULL
 */
void list_init(struct list* list, lfree_t destroyer);
/**
 * Verifica si la lista está vacia.
 *
 * @return true si está vacio, false lo contrario
 */
bool list_empty(struct list* list);
/**
 * Destruye una lista ya inicializada.
 *
 * @param list la lista a destruir
 */
void list_destroy(struct list* list);

/**
 * Añade un elemeneto al comienzo de la lista.
 *
 * @param list la lista en donde se añadira el elemento
 * @param data el elemento a añadir
 */
void list_add_head(struct list* list, void* data);
/**
 * Añade un elemeneto al final de la lista.
 *
 * @param list la lista en donde se añadira el elemento
 * @param data el elemento a añadir
 */
void list_add_last(struct list* list, void* data);

/**
 * Elimina el primer elemento de la lista.
 *
 * @param list la lista a quitar el elemento
 * @return el valor que tenía
 */
void* list_del_head(struct list* list);
/**
 * Elimina el último elemento de la lista.
 *
 * @param list la lista a quitar el elemento
 * @return el valor que tenía
 */
void* list_del_last(struct list* list);

/**
 * Inicializa una cola.
 *
 * @param queue la cola a inicializar
 * @param destroyer destructor de valores, puede ser NULL
 */
static inline void queue_init(queue_t* queue, lfree_t destroyer) {
    list_init(queue, destroyer);
}

/**
 * Verifica si la cola está vacia.
 *
 * @return true si está vacio, false lo contrario
 */
static inline bool queue_empty(queue_t* queue) {
    return list_empty(queue);
}

/**
 * Destruye una cola ya inicializada.
 *
 * @param queue la cola a destruir
 */
static inline void queue_destroy(queue_t* queue) {
    list_destroy(queue);
}

/**
 * Añade un elemento a la cola.
 *
 * @param queue la cola a añadir el elemento
 * @param data el elemento a añadir
 */
static inline void queue_add(queue_t* queue, void* data) {
    list_add_last(queue, data);
}

/**
 * Elimina el siguiente elemento de la cola.
 *
 * @param queue la cola a quitar el elemento
 */
static inline void* queue_del(queue_t* queue) {
    return list_del_head(queue);
}

/**
 * Inicializa una pila.
 *
 * @param stack la pila a inicializar
 * @param destroyer destructor de valores, puede ser NULL
 */
static inline void stack_init(stack_t* stack, lfree_t destroyer) {
    list_init(stack, destroyer);
}

/**
 * Verifica si la pila está vacia.
 *
 * @param stack la pila a verificar
 * @return true si está vacia, falso lo contrario
 */
static inline bool stack_empty(stack_t* stack) {
    return list_empty(stack);
}

/**
 * Destroye una pila ya inicializada.
 *
 * @param stack la pila a destruir
 */
static inline void stack_destroy(stack_t* stack) {
    list_destroy(stack);
}

/**
 * Añade un elemento encima de la pila.
 *
 * @param stack la pila en donde se añadirá
 * @param data el eleme
 */
static inline void stack_push(stack_t* stack, void* data) {
    list_add_last(stack, data);
}

/**
 * Quita el elemento encima de la pila.
 *
 * @param stack la pila a quitar el elemento
 * @return el elemento que tenía
 */
static inline void* stack_pop(stack_t* stack) {
    return list_del_last(stack);
}

#endif // ED_STACK_GUARD_HEADER
