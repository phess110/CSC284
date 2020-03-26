#include "SegmentTree.h"


SegmentTree::SegmentTree(vector<uint32_t> endpoints) {
    tree.resize(1 << static_cast<uint32_t>(ceil(log2(endpoints.size())) + 1));
    BuildTree(endpoints, 0, 0, endpoints.size() - 1);
}

void SegmentTree::BuildTree(vector<uint32_t> endpoints, size_t loc, size_t start, size_t end) {
    uint32_t mid = static_cast<uint32_t>(ceil((end - start)/ 2.));
    tree[loc].left = endpoints[start];
    tree[loc].right = endpoints[end];
    
    //for (int i = 0; i < d; i++) {
    //    std::cout << " ";
    //}
    //std::cout << loc << "," << start << "," << end << std::endl;

    if (end > start + 1) {
        BuildTree(endpoints, lchild(loc), start, start + mid);
        BuildTree(endpoints, rchild(loc), start + mid, end);
    }
    else {
        tree[loc].isLeaf = true;
    }
}

uint32_t SegmentTree::Insert(uint32_t x, uint32_t y) {
    Update(0, x, y, 1);
    return tree[0].measure;
}

uint32_t SegmentTree::Delete(uint32_t x, uint32_t y) {
    Update(0, x, y, -1);
    return tree[0].measure;
}

void SegmentTree::Update(size_t loc, uint32_t x, uint32_t y, int32_t c) {
    if (x == tree[loc].left && y == tree[loc].right) {
        tree[loc].count += c;
    }
    else {
        uint32_t m = tree[lchild(loc)].right;
        if (x < m && m < y) {
            Update(lchild(loc), x, m, c);
            Update(rchild(loc), m, y, c);
        }
        else if (y <= m) {
            Update(lchild(loc), x, y, c);
        }
        else if (m <= x) {
            Update(rchild(loc), x, y, c);
        }
    }

    if (tree[loc].count == 0) {
        if (tree[loc].isLeaf) {
            tree[loc].measure = 0;
        }
        else {
            tree[loc].measure = tree[lchild(loc)].measure + tree[rchild(loc)].measure;
        }
    }
    else {
        tree[loc].measure = tree[loc].right - tree[loc].left;
    }
}


int main() {
    uint32_t m, n;
    char op;
    std::cin >> m;
    std::cin >> n;

    std::vector<uint32_t> ends;

    uint32_t t;
    for (uint32_t i = 0; i < m; i++) {
        std::cin >> t;
        ends.push_back(t);
    }

    SegmentTree *T = new SegmentTree(ends);

    uint32_t l, r;
    for (uint32_t i = 0; i < n; i++) {
        std::cin >> op;
        std::cin >> l;
        std::cin >> r;

        if (op == 'I') {
            std::cout << T->Insert(l, r) << std::endl;
        }
        else {
            std::cout << T->Delete(l, r) << std::endl;
        }
    }
}