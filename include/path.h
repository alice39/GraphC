#ifndef ED_PATH_GUARD_HEADER
#define ED_PATH_GUARD_HEADER

#include <stdint.h>

#include "vertex.h"
#include "map.h"

/**
 * Represents a path from a source vertice until a destination
 * vertex.
 *
 * @see gpath_destroy
 *
 * @member vertices is the vertex (vertex_t) sequence
 * @member weight is the final weight that will take from
 *                source vertex until destination vertex
 */
struct path {
    struct vertex_array vertices;
    int32_t weight;
};

/**
 * Represents a HashMap that links uin32_t (the key) with a
 * struct path (the value).
 *
 * Seen in another way: HashMap<uint32_t, struct path>
 *
 * @see hashmap
 * @see u32path_destroyer
 */
typedef struct hashmap u32path_map;

/**
 * Initialize a path with the vertices that compose it
 *
 * @param path the path to initialize
 * @param vertices the vertex sequence
 */
void path_init(struct path* path, struct vertex_array* vertices);
/**
 * Destroy an initialized path.
 *
 * @param path the path to destroy
 */
void path_destroy(struct path* path);

/**
 * The value destructor of a u32path_map hashmap.
 *
 * @see hashmap#destroyer
 */
void u32path_destroyer(void* path);

#endif // ED_PATH_GUARD_HEADER
