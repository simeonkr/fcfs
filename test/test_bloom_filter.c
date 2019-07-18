#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../bloom_filter.h"

void test_bf_simple() {
    bloom_filter bf = bloom_filter_new(h_string, 10, 0.1);
    
    for (int i = 0; i < 10; i++) {
        char key[5];
        sprintf(key, "str%d", i);
        
        bloom_filter_insert(&bf, (hash_key){.p=key});
        assert(bloom_filter_lookup(&bf, (hash_key){.p=key}) == 1);
    }
    
    int f = 1;
    for (int i = 10; i < 20; i++) {
        char key[6];
        sprintf(key, "str%d", i);
        f &= bloom_filter_lookup(&bf, (hash_key){.p=key});
    }
    // there is a 1 in 10 billion chance of this assert failing when it shouldn't
    assert(f == 0);
    
    for (int i = 10; i < 100; i++) {
        char key[6];
        sprintf(key, "str%d", i);
        f |= bloom_filter_lookup(&bf, (hash_key){.p=key});
    }
    // 0.9^90 = 7.6e-5 (1 in 13127) chance here
    assert(f == 1);
    
    bloom_filter_free(&bf);
}

void test_bf_stats() {
    float p = 0.1;
    int T = 1000;
    for (int i = 1; i <= 3; i++) {
        int n = pow(10, i);
        
        bloom_filter bf = bloom_filter_new(h_int, n, p);
        
        for (int i = 0; i < n; i++) {
            int r = rand();
            bloom_filter_insert(&bf, (hash_key){.i=r});
            assert(bloom_filter_lookup(&bf, (hash_key){.i=r}) == 1);
        }
        
        int sum = 0;
        for (int i = 0; i < T; i++) {
            int r = rand();
            sum += bloom_filter_lookup(&bf, (hash_key){.i=r});
        }
        // sum ~ binom(T, p) or approx. norm(T*p, T*p*(1-p))
        float mean = T*p;
        float std = sqrt(T*p*(1-p));
        
        // verify manually: this is too lenient
        assert (sum >= mean - 4*std && sum <= mean + 4*std);
        
        bloom_filter_free(&bf);
    }
}

int main(int argc, char *argv[]) {
    test_bf_simple();
    test_bf_stats();

    printf("%s: All tests passed.\n", argv[0]);
}
