#include "SuffixArray.h"

void BuildSuffixArray() {
    uint32_t k = 1;
    while (k < len) {
        // build the tuples from A
        for (uint32_t i = 0; i < len; i++) {
            if (A[i] == 0) {
                tuples[i]->msb = 0;
                tuples[i]->lsb = 0;
            }
            else {
                tuples[i]->msb = A[i];
                tuples[i]->lsb = A[MIN(i+k, len - 1)];
            }
            tuples[i]->index = i;
        }

        TupleSort();

        // use sorted tuples to compute A'
        uint32_t j = 0; // uniqueness counter
        uint32_t prevMSD = 0, prevLSD = 0;
        for (uint32_t i = 0; i < len; i++) {
            // Compress
            if (prevLSD != tuples[i]->lsb || prevMSD != tuples[i]->msb) {
                prevLSD = tuples[i]->lsb;
                prevMSD = tuples[i]->msb;
                j++;
            }

            A[tuples[i]->index] = j;
        }
        
        k <<= 1;
    }

    // invert array
    for (uint32_t i = 0; i < len; i++) {
        SuffixArray[A[i]] = i;
    }
}

int main() {
    uint32_t n;
    std::cin >> n;

    for (uint32_t i = 0; i < MAX_LEN; i++) {
        tuples[i] = new SuffixTuple();
    }
   
    std::string t; 
    for (uint32_t i = 0; i < n; i++) {
        // read in string
        std::cin >> t;
        t += "$";
        len = t.length();
        
        // translate
        for (uint32_t i = 0; i < len; i++) {
            A[i] = CharToDigit(t[i]);
        }

        // build suffix array
        BuildSuffixArray();

        // output array
        for (uint32_t i = 0; i < len; i++) {
            std::cout << SuffixArray[i] << " ";
        }
        std::cout << std::endl;
    }
}