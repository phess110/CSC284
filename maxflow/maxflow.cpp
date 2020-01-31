/**
    Max flow main
**/

#include <iostream>
#include "Graph.h"
#include "Vertex.h"
#include <string>

int problemCount;

// todo add mechanism to switch between two heuristics

uint32_t ReadFlowNetwork() {
    uint16_t order, size, source, sink;

    std::cin >> order;
    std::cin >> size;
    std::cin >> source;
    std::cin >> sink;

    Graph g(order);
    uint16_t v, u;
    uint32_t c;
    for (int i = 1; i <= size; i++) {
        std::cin >> v;
        std::cin >> u;
        std::cin >> c;

        g.AddEdgeNoReversal(v, u, c);
    }
    
    //return g.EdmondKarp(source, sink);
    return g.PushRelabelFlow(source, sink);
}

int main()
{
    std::cin >> problemCount;

    for (int i = 0; i < problemCount; i++) {
        std::cout << ReadFlowNetwork() << std::endl;
    }

    return 0;
}