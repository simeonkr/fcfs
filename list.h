#ifndef LIST_H
#define LIST_H

#include <stdlib.h>

typedef struct list_node {
    void *data;
    struct list_node *next;
    struct list_node *prev;
} list_node;

typedef struct {
    list_node *head;
    list_node *tail;
    int size;
} list;

typedef int (*node_comp_func)(void *a, void *b);

typedef void (*data_free_func)(void *data);

void default_free(void *data);

list list_new();

list_node *list_lookup(list *l, void *data, node_comp_func func);

void list_append(list *l, void *data);

void list_insert(list *l, void *data, node_comp_func func);

int list_replace(list *l, void *data, node_comp_func func);

int list_remove(list *l, void *data, node_comp_func func, data_free_func free_func);

int list_size(list *l);

void list_free(list *l, data_free_func free_func);

#endif /* LIST_H */
