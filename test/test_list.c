#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../list.h"

int data_strcmp(void *a, void *b) {
    return strcmp((char *)a, (char *)b) == 0;
}

char *alloc_str(char *c) {
    char *out = malloc(strlen(c) + 1);
    strncpy(out, c, strlen(c) + 1);
    return out;
}

list make_short_list() {
    list l = list_new();
    
    list_append(&l, alloc_str("str1"));
    list_append(&l, alloc_str("str2"));
    list_append(&l, alloc_str("str3"));
    
    return l;
}

void test_list_append() {
    list l = make_short_list();
    
    assert(data_strcmp(l.head->data, "str1"));
    assert(data_strcmp(l.head->next->data, "str2"));
    assert(data_strcmp(l.tail->data, "str3"));
    assert(data_strcmp(l.tail->prev->data, "str2"));
    assert(list_size(&l) == 3);
    
    list_free(&l, default_free);
}

void test_list_lookup() {
    list l = make_short_list();
    
    list_node *node = list_lookup(&l, "str1", data_strcmp);
    assert(node == l.head);
    
    node = list_lookup(&l, "str3", data_strcmp);
    assert(node == l.tail);
    
    node = list_lookup(&l, "str2", data_strcmp);
    assert(data_strcmp(node->data, "str2"));
    assert(data_strcmp(node->prev->data, "str1"));
    assert(data_strcmp(node->next->data, "str3"));
    
    node = list_lookup(&l, "str4", data_strcmp);
    assert(node == NULL);
    
    list_free(&l, default_free);
}

int at_head(void *a, void *b) {
    return 1;
}

int at_tail(void *a, void *b) {
    return 0;
}

int after_str2(void *a, void *b) {
    return atoi((char *)a + 3) > 2;
}

void test_list_insert() {
    // 1,2,3
    list l = make_short_list();
    
    // 1,1,2,3
    list_insert(&l, alloc_str("str1"), data_strcmp);
    assert(data_strcmp(l.head->data, "str1"));
    assert(data_strcmp(l.head->next->data, "str1"));
    
    // 0.5,1,1,2,3
    list_insert(&l, alloc_str("str0.5"), at_head);
    assert(data_strcmp(l.head->data, "str0.5"));
    assert(data_strcmp(l.head->next->data, "str1"));
    
    // 0.5,1,1,2,3,4
    list_insert(&l, alloc_str("str4"), at_tail);
    assert(data_strcmp(l.tail->data, "str4"));
    assert(data_strcmp(l.tail->prev->data, "str3"));
    
    // 0.5,1,1,2,2.5,3,4
    list_insert(&l, alloc_str("str2.5"), after_str2);
    list_node *node = list_lookup(&l, "str2", data_strcmp);
    assert(data_strcmp(node->next->data, "str2.5"));
    assert(data_strcmp(node->next->next->data, "str3"));
    
    assert(list_size(&l) == 7);
    
    list_free(&l, default_free);
    
    l = list_new();
    // 1
    list_insert(&l, alloc_str("str1"), data_strcmp);
    assert(data_strcmp(l.head->data, "str1"));
    assert(data_strcmp(l.tail->data, "str1"));
    
    list_free(&l, default_free);
}

void test_list_replace() {
    list l = make_short_list();
    
    // 1x,2,3
    list_replace(&l, alloc_str("str1x"), at_head);
    // 1x,2,3x
    list_replace(&l, alloc_str("str3x"), after_str2);
    // 1x,2,3x,4
    list_replace(&l, alloc_str("str4"), data_strcmp);
    
    assert(data_strcmp(l.head->data, "str1x"));
    assert(data_strcmp(l.head->next->data, "str2"));
    assert(data_strcmp(l.tail->prev->data, "str3x"));
    assert(data_strcmp(l.tail->data, "str4"));
    
    assert(list_size(&l) == 4);
    
    list_free(&l, default_free);
}

void test_list_remove() {
    list l = make_short_list();
    
    // 1,2,3
    list_remove(&l, "str4", data_strcmp, default_free);
    // 1,3
    list_remove(&l, "str2", data_strcmp, default_free);
    assert(data_strcmp(l.head->data, "str1"));
    assert(data_strcmp(l.head->next->data, "str3"));
    assert(data_strcmp(l.tail->data, "str3"));
    
    // 3
    list_remove(&l, "str1", data_strcmp, default_free);
    assert(data_strcmp(l.head->data, "str3"));
    assert(data_strcmp(l.tail->data, "str3"));
    
    // 1,3
    list_append(&l, alloc_str("str1"));
    // 1
    list_remove(&l, "str3", data_strcmp, default_free);
    assert(data_strcmp(l.head->data, "str1"));
    assert(data_strcmp(l.tail->data, "str1"));
    
    // empty
    list_remove(&l, "str1", data_strcmp, default_free);
    assert(l.head == NULL);
    assert(l.tail == NULL);
    assert(list_size(&l) == 0);
    
    list_free(&l, default_free);
}

void test_list_traverse() {
    list l = make_short_list();
    
    char *s = malloc(5 * sizeof(char));
    int i;
    
    i = 0;
    for (list_node *cur = l.head; cur != NULL; cur = cur->next, i++) {
        sprintf(s, "str%d", i + 1);
        assert(data_strcmp(cur->data, s));
    }
    
    i = 2;
    for (list_node *cur = l.tail; cur != NULL; cur = cur->prev, i--) {
        sprintf(s, "str%d", i + 1);
        assert(data_strcmp(cur->data, s));
    }
    
    free(s);
    list_free(&l, default_free);
}

int main(int argc, char *argv[]) {
    test_list_append();
    test_list_lookup();
    test_list_insert();
    test_list_replace();
    test_list_remove();
    test_list_traverse();

    printf("%s: All tests passed.\n", argv[0]);
}
