#include <stdlib.h>
#include <string.h>
#include "hash.h"
#include "list.h"

// https://planetmath.org/goodhashtableprimes
static int primes[] = {
    53, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317, 196613,
    393241, 786433, 1572869, 1572869, 3145739, 62916491, 12582917, 25165843,
    50331653, 100663319, 201326611, 402653189, 805306457, 1610612471
};
                            
int h_int(hash_key k, int m) {
    return (unsigned int)(k.i) % m;
}

// lcg-based hash with mult. constant a=31
int h_string(hash_key k, int m) {
    unsigned int h = 0;
    for (char *s = k.p; *s != '\0'; s++) {
        h = ((h << 5) - h + *s) % m;
    }
    return h;
}

int key_int_comp(void *a, void *b) {
    hash_entry *ha = a;
    hash_entry *hb = b;
    return ha->key.i == hb->key.i;
}

int key_string_comp(void *a, void *b) {
    hash_entry *ha = a;
    hash_entry *hb = b;
    return strcmp((char *)(ha->key.p), (char *)(hb->key.p)) == 0;
}

void hash_entry_free_k(void *data) {
    hash_entry *entry = data;
    free(entry->key.p);
    free(entry);
}

hash_table hash_table_new(hash_func h, node_comp_func cmp) {
    int init_size = primes[0];
    return (hash_table){
        .table_size = init_size, 
        .table_size_i = 0, 
        .num_entries = 0, 
        .table = calloc(init_size, sizeof(list)), 
        .h = h, 
        .h_comp = cmp};
}

static void hash_table_expand(hash_table *ht) {
    list *old_table = ht->table;
    int old_size = ht->table_size;
    
    ht->table_size = primes[++ht->table_size_i];
    ht->table = calloc(ht->table_size, sizeof(list));
    
    for (int i = 0; i < old_size; i++) {
        for (list_node *cur = old_table[i].head; cur != NULL;) {
            list_node *cur_cp = cur;
            cur = cur->next;
            
            hash_entry *entry = cur_cp->data;
            free(cur_cp);
            int slot = ht->h(entry->key, ht->table_size);
            list_append(&ht->table[slot], entry);
        }
    }
    
    free(old_table);
}

void hash_table_insert(hash_table *ht, hash_key key, hash_val val) {
    hash_entry *entry = malloc(sizeof(hash_entry));
    entry->key = key;
    entry->val = val;
    
    int slot = ht->h(key, ht->table_size);
    ht->num_entries += list_replace(&ht->table[slot], entry, ht->h_comp);
    
    if ((4*ht->num_entries) / (3*ht->table_size) >= 1) {  // load factor > 75%
        hash_table_expand(ht);
    }
}

static hash_entry make_test_entry(hash_key key) {
    return (hash_entry){key, (hash_val){.p = NULL}}; 
} 

void hash_table_remove(hash_table *ht, hash_key key, data_free_func free_func) {
    int slot = ht->h(key, ht->table_size);
    hash_entry test_entry = make_test_entry(key);
    ht->num_entries += list_remove(
        &ht->table[slot], &test_entry, ht->h_comp, free_func);
}

hash_entry *hash_table_lookup(hash_table *ht, hash_key key) {
    int slot = ht->h(key, ht->table_size);
    hash_entry test_entry = make_test_entry(key);
    list_node *node = list_lookup(&ht->table[slot], &test_entry, ht->h_comp);
    
    return (node == NULL) ? NULL : node->data;
}

hash_entry *hash_table_lookup_insert(hash_table *ht, hash_key key, hash_val val) {
    int slot = ht->h(key, ht->table_size);
    hash_entry test_entry = make_test_entry(key);
    list_node *node = list_lookup(&ht->table[slot], &test_entry, ht->h_comp);
    
    if (node == NULL) {
        hash_entry *entry = malloc(sizeof(hash_entry));
        entry->key = key;
        entry->val = val;
        list_append(&ht->table[slot], entry);
        ht->num_entries++;
        
        if ((4*ht->num_entries) / (3*ht->table_size) >= 1) {
            hash_table_expand(ht);
        }
        
        return NULL;
    }
    return node->data;
}

void hash_table_free(hash_table *ht, data_free_func free_func) {
    for (int i = 0; i < ht->table_size; i++) {
        list_free(&ht->table[i], free_func);
    }
    free(ht->table);
}
