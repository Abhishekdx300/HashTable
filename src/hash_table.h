#ifndef HASH_TABLE
#define HASH_TABLE

typedef struct {
  char *key;
  char *value;
} item;

typedef struct {
  int size;
  int count;
  int base_size;
  item **items;
} hash_table;

void ht_insert(hash_table* ht, const char* key, const char* value);
char* ht_search(hash_table* ht, const char* key);
void ht_delete(hash_table* ht, const char* key);



#endif
