#pragma once
#include <cstdint>
#include <iostream>
#include <math.h>
#include <vector>

// Heap indexing
#define parent(k) ((k - 1) >> 1)
#define lchild(k) ((k << 1) + 1)
#define rchild(k) ((k << 1) + 2)

using std::vector;

class SegmentTree {

    class SegmentTreeNode {
    public:
        uint32_t left, right, measure, count;
        bool isLeaf;
        SegmentTreeNode() : left(0), right(0), measure(0), count(0), isLeaf(false) {};
    };

public:
    vector<SegmentTreeNode> tree;
    uint32_t Insert(uint32_t, uint32_t);
    uint32_t Delete(uint32_t, uint32_t);
    SegmentTree(vector<uint32_t>);
private:
    void Update(size_t, uint32_t, uint32_t, int32_t);
    void BuildTree(vector<uint32_t>, size_t, size_t, size_t);
};

