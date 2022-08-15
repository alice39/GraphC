#ifndef ED_WAVE_GUARD_HEADER
#define ED_WAVE_GUARD_HEADER

#include <stdint.h>

#include "list.h"
#include "map.h"

#include "vertex.h"
#include "path.h"

/**
 * Represents a linked list of struct wave
 */
typedef struct list wave_list;

/**
 * Represents a wave from a graph.
 *
 * @member parent the parent wave, NULL if there is no
 * @member depth the wave depth
 * @member vertex the vertex of this wave
 * @member subwaves the underlying sub-waves
 */
struct wave {
    struct wave* parent;

    size_t depth;
    vertex_t vertex;

    wave_list subwaves;
};

/**
 * Represents a HashMap that links a vertex_t (the key) with a
 * struct wave (the value).
 *
 * See in another way: HashMap<vertex_t, struct wave>
 *
 * @see wave_destroyer
 */
typedef struct hashmap vtwave_map;

/**
 * Initialize a wave.
 *
 * @param wave the wave to initialize
 * @param parent the parent wave, NULL if there is no
 * @param vertex the vertex of this wave
 */
void wave_init(struct wave* wave, struct wave* parent, vertex_t vertex);
/**
 * Print in STDOUT a wave.
 *
 * @param wave the wave to print
 */
void wave_print(struct wave* wave);
/**
 * Destroy an initialized wave.
 *
 * @param wave the wave to destroy
 */
void wave_destroy(struct wave* wave);

/**
 * Add a vertex in a wave.
 *
 * If a vertex is attempted to be added twice, it will be
 * ignored.
 *
 * @param wave the wave where to add the vertex in
 * @param vertex the vertex to add
 * @return the sub-wave created by adding the vertex
 */
struct wave* wave_add(struct wave* wave, vertex_t vertex);
/**
 * Return the sub-wave of a vertex.
 *
 * @param wave the wave where to get the sub-wave
 * @param vertex the vertex to look for
 * @return the vertex's sub-wave, NULL if it doesn't exist
 */
struct wave* wave_get(struct wave* wave, vertex_t vertex);

/**
 * Transform a wave in all possible paths.
 *
 * @param wave the wave to transform
 * @param out_map the generated paths
 */
void wave_to_path(struct wave* wave, u32path_map* out_map);

/**
 * The value destructor of wave.
 *
 * @param wave the wave to destroy
 */
void wave_destroyer(void* wave);

#endif // ED_WAVE_GUARD_HEADER
