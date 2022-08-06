#ifndef ED_HASHMAP_GUARD_HEADER
#define ED_HASHMAP_GUARD_HEADER

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * El factor de carga máxima por defecto
 */
#define MAP_DEFAULT_MAX_LOAD_FACTOR 1.0f

/**
 * Representa la llave del mapa.
 */
typedef size_t mkey_t;
/**
 * Representa el tamaño del mapa.
 */
typedef size_t msize_t;
/**
 * Representa el destructor de valores del mapa.
 */
typedef void (*mfree_f)(void* value);

/**
 * Representa el par que relaciona la llave con un valor.
 *
 * @member key la llave usada
 * @member value el dato guardado
 */
struct map_entry {
    mkey_t key;
    void* value;
};

/**
 * Representa un nodo de un struct map_entry en una
 * lista doble-enlazada.
 *
 * El nodo sirve para evitar posibles colisiones en el mapa.
 *
 * @member entry el par que relaciona la llave con un dato
 * @member back el nodo previo (subyancete a este nodo), NULL
 *              si no hay (entonces el primer nodo)
 * @member next el nodo siguiente (subyancete a este nodo),
 *              NULL si no hay (entonces el último nodo)
 */
struct hashmap_node {
    struct map_entry entry;

    struct hashmap_node* next;
};

/**
 * La estructura de dato que representa a un HashMap.
 *
 * Visto de otra forma: HashMap<Key, Value>
 *
 * @see hashmap_init
 * @see hashmap_destroy
 *
 * @member reserved la cantidad de espacio reservado en el
 *                  mapa
 * @member size la cantidad de objetos añadidos al mapa
 * @member max_load_factor el factor que indica cuando se
 *                         debería de doblar el tamaño
 * @member destroyer la función que destruye el valor
 *                   almacenado en el mapa, puede ser NULL
 *                   si no es necesario
 * @member bucks es el arreglo de nodos en que se reservó
 *               espacio
 */
struct hashmap {
    msize_t reserved;
    msize_t size;
    float max_load_factor;

    mfree_f destroyer;

    struct hashmap_node** bucks;
};

/**
 * El iterator de un HashMap.
 *
 * @see hashmap_iterator_init
 *
 * @member map el mapa en el que se está iterando
 * @member pos la posición siguiente del iterador
 * @member node el nodo siguiente a iterar, NULL si no hay
 * @member found_size el número de elementos encontrados
 *                    durante la iteración actual
 */
struct hashmap_iterator {
    const struct hashmap* map;
    msize_t pos;
    const struct hashmap_node* node;

    msize_t found_size;
};

/**
 * Inicializa un hashmap con una capacidad inicial.
 *
 * @param map el hashmap a inicializar
 * @param initial_capacity capacidad a reservar
 * @param destroyer destructor de valores, puede ser NULL
 */
void hashmap_init(struct hashmap* map, msize_t initial_capacity, mfree_f destroyer);
/**
 * Reserva más capacidad a un hashmap.
 *
 * Si la capacidad es 0, no se aplicará ninguna acción.
 *
 * @param map el hashmap a reservar espacio
 * @param capacity la capacidad a reservar
 */
void hashmap_reserve(struct hashmap* map, msize_t capacity);
/**
 * Destruye un ya inicializado hashmap.
 *
 * @param map el mapa de destruir
 */
void hashmap_destroy(struct hashmap* map);

/**
 * La cantidad de valores que hay en el hashmap.
 *
 * @param map el mapa a obtener el tamaño
 * @return la cantidad de valores que hay
 */
msize_t hashmap_size(const struct hashmap* map);

/**
 * Añade un valor con una llave dada en un hashmap.
 *
 * @param map el hashmap a añadir el par de objetos
 * @param key la llave a vincular con el valor
 * @param value el valor a añadir
 * @return el valor previo que estaba vinculado a la llave
 */
void* hashmap_put(struct hashmap* map, mkey_t key, void* value);
/**
 * Comprueba que una llave ya está añadida en un hashmap.
 *
 * @param map el hashmap a buscar la llave
 * @param key la llave a comprobar
 * @return true si existe de lo contrario false
 */
bool hashmap_has(const struct hashmap* map, mkey_t key);
/**
 * Regresa el valor vinculado con la llave en un hashmap.
 *
 * Si no existe ninguna llave en el hashmap se devolverá
 * NULL.
 *
 * @see hashmap_has
 *
 * @param map el hashmap a buscar el valor
 * @param key la llave vinculada al valor
 * @return el valor encontrado
 */
void* hashmap_get(const struct hashmap* map, mkey_t key);
/**
 * Elimina una llave de un hashmap.
 *
 * @param map el hashmap donde se va a eliminar la llave
 * @param key la llave a buscar y eliminar
 * @return el valor vinculado a la llave
 */
void* hashmap_del(struct hashmap* map, mkey_t key);

/**
 * Inicializa un iterator de hashmap.
 *
 * @param it el iterator a inicializar
 * @param map el hashmap donde se iterará
 */
void hashmap_iterator_init(struct hashmap_iterator* it, const struct hashmap* map);
/**
 * Itera hacía al siguiente elemento.
 *
 * @param it el iterator donde se buscará el siguiente
 *           elemento
 * @param out_entry el elemento encontrado
 * @return true si pudo conseguir otro elemento de lo
 *              contrario false
 */
bool hashmap_iterator_next(struct hashmap_iterator* it, struct map_entry* out_entry);

#endif // ED_MAP_GUARD_HEADER
