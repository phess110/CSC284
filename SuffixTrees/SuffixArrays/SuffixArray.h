#pragma once

#include <cstdint>
#include <stdint.h>
#include <iostream>
#include <vector>
#include <list>

#define MIN(a,b) ((a) < (b) ? (a) : (b))
constexpr auto MAX_LEN = 100001;
constexpr auto NUM_CHARS = 27;
using std::list;
using std::vector;

typedef struct SuffixTuple {
    uint8_t msb, lsb;
    uint32_t index;
} SuffixTuple;

uint32_t len;

// Tuples 
vector<SuffixTuple *> tuples(MAX_LEN);
// Storage for suffix array
uint32_t A[MAX_LEN];
// Storage for suffix array
uint32_t SuffixArray[MAX_LEN];

// Buckets for sorting
vector<list<SuffixTuple *>> buckets(NUM_CHARS);

uint8_t CharToDigit(char a) {
    if (a == '$') {
        return 0;
    }
    else {
        return a - 96;
    }
}

void BucketSort(bool sortByMSD) {
    for (uint32_t i = 0; i < len; i++) {
        if (sortByMSD) {
            buckets[tuples[i]->msb].push_back(tuples[i]);
        }
        else {
            buckets[tuples[i]->lsb].push_back(tuples[i]);
        }
    }

    // iterate over buckets, put back into tuples array
    uint32_t j = 0;
    for (uint32_t i = 0; i < NUM_CHARS; i++) {
        list<SuffixTuple *>::iterator it = buckets[i].begin();
        for (it; it != buckets[i].end(); it++) {
            tuples[j] = *it;
            j++;
        }
        buckets[i].clear();
    }
}

void TupleSort() {
    BucketSort(false);
    BucketSort(true);
}

