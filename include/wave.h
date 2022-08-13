#ifndef ED_WAVE_GUARD_HEADER
#define ED_WAVE_GUARD_HEADER

#include <stdint.h>

#include "list.h"
#include "vertex.h"

/**
 * Representa una lista enlazada de struct wave;
 */
typedef struct list wave_list;

/**
 * Representa una onda de un grafo.
 *
 * @member parent la onda anterior, NULL si no hay
 * @member depth la profundidad de la onda
 * @member vertex el vertice de esta onda
 * @member subwaves los sub-ondas existentes
 */
struct wave {
    struct wave* parent;

    size_t depth;
    vertex_t vertex;

    wave_list subwaves;
};

/**
 * Inicializa una onda.
 *
 * @param wave la onda a inicializar
 * @param parent la onda anterior, NULL si no hay
 * @param vertex el vertice de esta onda
 */
void wave_init(struct wave* wave, struct wave* parent, vertex_t vertex);
/**
 * Imprime en consola una onda.
 *
 * @param wave la onda a imprimir
 */
void wave_print(struct wave* wave);
/**
 * Destroye una onda inicializada.
 *
 * @param wave la onda a destruir
 */
void wave_destroy(struct wave* wave);

/**
 * Añade un vertice a la onda.
 *
 * Si se intenta añadir un vertice dos veces, se ignorará.
 *
 * @param wave la onda donde se añadira el vertice
 * @param vertex el vertice a añadir
 * @return la onda del vertice que fue añadida
 */
struct wave* wave_add(struct wave* wave, vertex_t vertex);
/**
 * Regresa la onda de un vertice.
 *
 * @param wave la onda en donde está el vertice
 * @param vertex el vertice a buscar
 * @return la onda del vertice, NULL si no existe
 */
struct wave* wave_get(struct wave* wave, vertex_t vertex);

/**
 * El destructor del wave para los tipos de datos.
 *
 * @param wave la onda
 */
void wave_destroyer(void* wave);

#endif // ED_WAVE_GUARD_HEADER
