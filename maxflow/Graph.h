#pragma once
#include <cstdint>
#include <map>
#include <queue>
#include <vector>
#include <unordered_map>
#include "Vertex.h"

using std::vector;
using std::map;
using std::priority_queue;
using std::queue;
using std::unordered_map;
using std::pair;

typedef map<pair<uint16_t, uint16_t>, uint32_t> function;
typedef pair<uint16_t, uint16_t> edge;

class Graph
{

public: 
    Graph(uint16_t);
    void AddEdge(uint16_t, uint16_t, uint32_t);
    uint32_t EdmondKarp(uint16_t, uint16_t);
    uint32_t PushRelabelFlow(uint16_t, uint16_t);
    pair<vector<uint16_t>, uint32_t> ShortestPath(uint16_t, uint16_t);

private:
    Graph ResidualNetwork(function);
    void AugmentResidualNetwork(Graph*, function, vector<uint16_t>);

    uint16_t order;
    unordered_map<uint16_t,Vertex> vertices;
    function capacity;
};

