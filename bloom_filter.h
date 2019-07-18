#ifndef BLOOM_FILTER_H
#define BLOOM_FILTER_H

#include "hash.h"

typedef struct {
    int m, k;
    char *bit_array;
    hash_func h;
} bloom_filter;

bloom_filter bloom_filter_new(hash_func h, int n, float p);

void bloom_filter_insert(bloom_filter *bf, hash_key key);

int bloom_filter_lookup(bloom_filter *bf, hash_key key);

void bloom_filter_free(bloom_filter *bf);

#endif /* BLOOM_FILTER_H */
