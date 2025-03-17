#include <stddef.h>
#include "const_strings.h"
#include "arena.h"

#define DEFAULT_R 31

typedef struct hashitem hashitem;
struct hashitem {
  hashitem *next;
  const_string key;
  void* val;
};

typedef struct {
  size_t cap;
  hashitem data[];
} hashmap;

size_t hash_str(const_string str, size_t cap) {
  size_t hash = 0;
  for (int i = 0; i < str.len; i++) {
	hash = (DEFAULT_R * hash + str.data[i]) % cap;
  }
  return hash;
}

hashmap *hashmap_init(arena *a, size_t cap) {
  hashmap *map = arena_alloc(a, sizeof(hashmap) + sizeof(ptrdiff_t) * cap);
  return map;
}

size_t hashmap_kstr_insert(arena *a, hashmap* map, const_string key, void* val) {
  size_t hash = hash_str(key, map->cap);
  if (map->data[hash].key.len > 0) {
	map->data[hash].next = arena_alloc(a, sizeof(hashitem));
	map->data[hash].next->key = key;
	map->data[hash].next->val = val;
  } else {
	map->data[hash].key = key;
	map->data[hash].val = val;
  }

  return hash;
}

void *hashmap_kstr_get(hashmap *map, const_string key) {
  size_t hash = hash_str(key, map->cap);
  hashitem *item = &map->data[hash];
  while (item->next != NULL && !cs_eq(item->key, key)) {
	item = item->next;
  }

  return item->val;
}


int main() {
  const_string key = CS("hello");
  const_string value = CS("world");

  arena a = {0};
  hashmap *map = hashmap_init(&a, 20);
  hashmap_kstr_insert(&a, map, key, (void *)&value);
  const_string *returned = hashmap_kstr_get(map, key);
  cs_print("%s", *returned);
}
