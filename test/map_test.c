#include <stdio.h>
#include <stdlib.h>

#include <map.h>

void simple_sample();
void million_sample();

int main() {
    simple_sample();
    million_sample();
    printf("Map Test Done.\n");

    return 0;
}

void simple_sample() {
    struct hashmap map = {0};
    hashmap_init(&map, 0, NULL);

    hashmap_put(&map, 5, "XD");
    hashmap_put(&map, 4, "Hola");
    hashmap_put(&map, -5, "Ooo");
    
    struct hashmap_iterator it;
    hashmap_iterator_init(&it, &map);

    for (struct map_entry entry; hashmap_iterator_next(&it, &entry);) {
        printf("%ld -> %s\n", entry.key, (char*) entry.value);
    }

    hashmap_destroy(&map);
}

void million_sample() {
    struct hashmap map = {0};
    hashmap_init(&map, 0, NULL);

    printf("%lu %lu\n", map.reserved, map.size);

    for (size_t i = 0; i < 1E6; i++) {
        hashmap_put(&map, i, NULL);
    }
    
    printf("%lu %lu\n", map.reserved, map.size);

    hashmap_destroy(&map);
}

