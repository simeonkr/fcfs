#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include "bloom_filter.h"
#include "hash.h"
#include <assert.h>

#define LN2 0.6931
#define LN2SQ 0.4805

int optimal_m(int n, float p) {
    return (int)(-n * log(p) / LN2SQ);
}

int optimal_k(float p) {
    return (int)(-log(p) / LN2);
}

bloom_filter bloom_filter_new(hash_func h, int n, float p) {
    bloom_filter bf;
    bf.m = optimal_m(n, p);
    bf.k = optimal_k(p);
    bf.bit_array = calloc((bf.m >> 3) + 1, sizeof(char));
    bf.h = h;
    return bf;
}

static int bf_h(bloom_filter *bf, hash_key key, int i) {
    /* dubious way of generating independent hash functions: 
       hash modulo numbers close to m (obviously a bad choice for small m) */
    return bf->h(key, bf->m + i) % bf->m;
}

void bloom_filter_insert(bloom_filter *bf, hash_key key) {
    for (int i = 0; i < bf->k; i++) {
        int bit_slot = bf_h(bf, key, i);
        // set bit @bit_slot to 1 if in_set else 0
        bf->bit_array[bit_slot >> 3] |= (1 << (bit_slot & 7));
        assert(bit_slot >> 3 < bf->m);
    }
}

int bloom_filter_lookup(bloom_filter *bf, hash_key key) {
    for (int i = 0; i < bf->k; i++) {
        int bit_slot = bf_h(bf, key, i);
        if (((bf->bit_array[bit_slot >> 3] >> (bit_slot & 7)) & 1) == 0) {
            return 0;
        }
        assert(bit_slot >> 3 < bf->m);
    }
    return 1;
}

void bloom_filter_free(bloom_filter *bf) {
    free(bf->bit_array);
}
