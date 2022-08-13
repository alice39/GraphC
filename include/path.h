#ifndef ED_PATH_GUARD_HEADER
#define ED_PATH_GUARD_HEADER

#include <stdint.h>

#include "vertex.h"
#include "map.h"

/**
 * Representa un camino de un vertice origen a un vertice
 * destino.
 *
 * @see gpath_destroy
 *
 * @member vertices es la secuencia de vertices (vertex_t) del
 *                  camino
 * @member weight es el peso final que toma el camino desde
 *                el vertice origen hasta el vertice destino
 */
struct path {
    struct vertex_array vertices;
    int32_t weight;
};

/**
 * Representa un HashMap en el que vincula uint32_t (la llave)
 * con un struct gpath (el valor).
 *
 * Visto de otra forma: HashMap<uint32_t, struct gpath>
 *
 * @see hashmap
 * @see u32path_destroyer
 */
typedef struct hashmap u32path_map;

/**
 * Inicializa un camino con valores por defecto.
 *
 * @param path la ruta a inicializar
 */
void path_zinit(struct path* path);
/**
 *  Destruye un camino ya inicializado.
 *
 *  @param path el camino destruir
 */
void path_destroy(struct path* path);

/**
 * El destructor del hashmap de u32path_map.
 *
 * @see hashmap#destroyer
 */
void u32path_destroyer(void* path);

#endif // ED_PATH_GUARD_HEADER
