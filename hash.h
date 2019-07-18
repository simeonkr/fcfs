#ifndef HASH_H
#define HASH_H

#include "list.h"

typedef union {
    void *p;
    char c;
    int i;
} hash_key, hash_val;

typedef int (*hash_func)(hash_key key, int m);

typedef struct {
    int table_size;
    int table_size_i;
    int num_entries;
    list *table;
    hash_func h;
    node_comp_func h_comp;
} hash_table;

typedef struct {
    hash_key key;
    hash_val val;
} hash_entry;
                 
int h_int(hash_key k, int m);

int h_string(hash_key k, int m);

int key_int_comp(void *a, void *b);

int key_string_comp(void *a, void *b);

void hash_entry_free_k(void *data);

hash_table hash_table_new(hash_func h, node_comp_func cmp);

void hash_table_insert(hash_table *ht, hash_key key, hash_val val);

void hash_table_remove(hash_table *ht, hash_key key, data_free_func free_func);

hash_entry *hash_table_lookup(hash_table *ht, hash_key key);

hash_entry *hash_table_lookup_insert(hash_table *ht, hash_key key, hash_val val);

void hash_table_free(hash_table *ht, data_free_func free_func);

#endif /* HASH_H */
