#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../hash.h"

hash_table make_small_ht() {
    hash_table ht = hash_table_new(h_string, key_string_comp);
    for (int i = 0; i < 10; i++) {
        char *key = malloc(5 * sizeof(char));
        sprintf(key, "str%d", i);
        hash_table_insert(&ht, (hash_key){.p=key}, (hash_val){.i=i});
    }
    return ht;
}

void test_hash_str() {
    char key1[5];
    char key2[5];
    for (int i = 0; i < 10; i++) {
        sprintf(key1, "str%d", i);
        sprintf(key2, "str%d", i);
        int h1 = h_string((hash_key){.p=key1}, i+1);
        int h2 = h_string((hash_key){.p=key2}, i+1);
        assert(h1 >= 0 && h1 < i+1 && h1 == h2);
    }
}

void test_hash_table_insert() {
    int found[10];
    for (int i = 0; i < 10; i++) {
        found[i] = 0;
    }
    
    hash_table ht = make_small_ht();
    for (int i = 0; i < ht.table_size; i++) {
        for (list_node *cur = ht.table[i].head; cur != NULL; cur = cur->next) {
            int j = ((hash_entry *)cur->data)->val.i;
            assert(j >= 0 && j < 10);
            assert(found[j] == 0);
            found[j] = 1;
        }
    }
    for (int i = 0; i < 10; i++) {
        assert(found[i] == 1);
    }
    
    assert(ht.num_entries == 10);
    
    hash_table_free(&ht, hash_entry_free_k);
}

void test_hash_table_remove() {
    int found[10];
    for (int i = 0; i < 10; i++) {
        found[i] = 0;
    }

    hash_table ht = make_small_ht();
    hash_table_remove(&ht, (hash_key){.p = "str1"}, hash_entry_free_k);
    hash_table_remove(&ht, (hash_key){.p = "str5"}, hash_entry_free_k);
    
    for (int i = 0; i < ht.table_size; i++) {
        for (list_node *cur = ht.table[i].head; cur != NULL; cur = cur->next) {
            int j = ((hash_entry *)cur->data)->val.i;
            assert(j >= 0 && j < 10);
            assert(found[j] == 0);
            found[j] = 1;
        }
    }
    for (int i = 0; i < 10; i++) {
        assert(found[i] == (i == 1 || i == 5 ? 0 : 1));
    }
    
    assert(ht.num_entries == 8);
    
    char key[5];
    for (int i = 0; i < 10; i++) {
        sprintf(key, "str%d", i);
        hash_table_remove(&ht, (hash_key){.p = key}, hash_entry_free_k);
    }
    
    for (int i = 0; i < ht.table_size; i++) {
        assert(ht.table[i].head == NULL);
    }
    
    assert(ht.num_entries == 0);
    
    hash_table_free(&ht, hash_entry_free_k);
}

void test_hash_table_lookup() {
    hash_table ht = make_small_ht();
    
    char key[5];
    for (int i = 0; i < 10; i++) {
        sprintf(key, "str%d", i);
        hash_entry *res = hash_table_lookup(&ht, (hash_key){.p = key});
        assert(res != NULL && res->val.i == i);
    }
    
    hash_table_free(&ht, hash_entry_free_k);
}

void test_hash_table_lookup_insert() {
    hash_table ht = make_small_ht();
    
    char key[5];
    for (int i = 0; i < 20; i++) {
        hash_entry *res, *lookup_res;
        if (i < 10) {
            sprintf(key, "str%d", i);
            res = hash_table_lookup_insert(&ht, 
                (hash_key){.p = key}, (hash_val){.i = i});
            assert(res != NULL && res->val.i == i);
        }
        else {
            char *a_key = malloc(6 * sizeof(char));
            sprintf(a_key, "str%d", i);
            res = hash_table_lookup_insert(&ht, 
                (hash_key){.p = a_key}, (hash_val){.i = i});
            lookup_res = hash_table_lookup(&ht, (hash_key){.p = a_key});
            assert(res == NULL && lookup_res->val.i == i);
        }
    }
    
    assert(ht.num_entries == 20);
    
    hash_table_free(&ht, hash_entry_free_k);
}

void test_hash_table_large() {
    hash_table ht = hash_table_new(h_int, key_int_comp);
    
    for (int i = 0; i < 10000; i++) {
        hash_table_insert(&ht, (hash_key){.i=i}, (hash_val){.i=i+7});
        assert((4*ht.num_entries) / (3*ht.table_size) < 1);
    }
    for (int i = 0; i < 10000; i++) {
        assert(hash_table_lookup(&ht, (hash_key){.i=i})->val.i == i+7);
    }
    
    assert(ht.num_entries == 10000);
    
    hash_table_free(&ht, default_free);
}

int main(int argc, char *argv[]) {
    test_hash_str();

    test_hash_table_insert();
    test_hash_table_remove();
    test_hash_table_lookup();
    test_hash_table_lookup_insert();
    test_hash_table_large();

    printf("%s: All tests passed.\n", argv[0]);
}
