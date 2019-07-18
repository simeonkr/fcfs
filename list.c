#include <stdlib.h>
#include <string.h>
#include "list.h"

void default_free(void *data) {
    free(data);
}

list list_new() {
    return (list){.head=NULL, .tail=NULL, .size=0};
}

static list_node **list_lookup_p(list *l, void *data, node_comp_func func) {
    list_node **cp = &l->head;
    list_node *cur = l->head;
    while (cur) {
        if (func(cur->data, data)) {
            return cp;
        }
        cp = &cur->next;
        cur = cur->next;
    }
    return NULL;
}

list_node *list_lookup(list *l, void *data, node_comp_func func) {
    list_node **np = list_lookup_p(l, data, func);
    return np == NULL ? NULL : *np;
}

void list_append(list *l, void *data) {
    list_node *new_node = malloc(sizeof(list_node));
    new_node->data = data;
    new_node->next = NULL;
    new_node->prev = l->tail;
    
    if (l->head == NULL) {
        l->head = new_node;
        l->tail = new_node;
    }
    else {
        l->tail->next = new_node;
        l->tail = new_node;
    }
    l->size++;
}

void list_insert(list *l, void *data, node_comp_func func) {   
    list_node **np = list_lookup_p(l, data, func);
    if (np != NULL) {
        list_node *new_node = malloc(sizeof(list_node));
        new_node->data = data;
        new_node->next = *np;
        new_node->prev = (*np)->prev;
        *np = new_node;
        l->size++;
    }
    else {
        list_append(l, data);
    }
}

int list_replace(list *l, void *data, node_comp_func func) {   
    list_node **np = list_lookup_p(l, data, func);
    if (np != NULL) {
        free((*np)->data);
        (*np)->data = data;
        return 0;
    }
    else {
        list_append(l, data);
        return 1;
    }
}

int list_remove(list *l, void *data, node_comp_func func, data_free_func free_func) {
    list_node **np = list_lookup_p(l, data, func);
    if (np != NULL) {
        list_node *node = *np;
        list_node *next = node->next;
        if (l->tail == node) {
            l->tail = l->tail->prev;
        }
        if (next != NULL) {
            next->prev = node->prev;
        }
        free_func(node->data);
        free(node);
        *np = next;
        l->size--;
        return -1;
    }
    else {
        return 0;
    }
}

int list_size(list *l) {
    return l->size;
}

void list_free(list *l, data_free_func data_free) {
    for (list_node *cur = l->head; cur != NULL;) {
        list_node *cur_cp = cur;
        cur = cur->next;

        data_free(cur_cp->data);
        free(cur_cp);
    }
}
