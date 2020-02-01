#pragma once
#include <algorithm>
#include <cstdint>
#include <map>
#include <queue>
#include <set>
#include <vector>
#include <unordered_map>
#include "Vertex.h"

using std::vector;
using std::map;
using std::priority_queue;
using std::queue;
using std::unordered_map;
using std::pair;

typedef map<pair<uint16_t, uint16_t>, uint32_t> unsigned_function;
typedef map<pair<uint16_t, uint16_t>, int32_t> signed_function;
typedef pair<uint16_t, uint16_t> edge;

class Graph {

public: 
    Graph(uint16_t);
    void AddEdge(uint16_t, uint16_t, uint32_t);
    void AddEdgeNoReversal(uint16_t, uint16_t, uint32_t);
    uint32_t EdmondKarp(uint16_t, uint16_t);
    uint32_t PushRelabelFlow(uint16_t, uint16_t);
    pair<vector<uint16_t>, uint32_t> ShortestPath(uint16_t, uint16_t);
    void AddVertex();

private:
    void Relabel(uint16_t, Graph &, vector<int> &);
    Graph ResidualNetwork(signed_function);
    void AugmentResidualNetwork(Graph*, signed_function &, vector<uint16_t>);

    uint16_t order;
    unordered_map<uint16_t, Vertex> vertices;
    // potential optimization - create edge class & store edge set
    // edge would store its endpoints, flow, capacity
    // would require efficient map from endpoints to edge
    unsigned_function capacity;
};

