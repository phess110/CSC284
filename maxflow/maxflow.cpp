//
//

#include <iostream>
#include "Graph.h"
#include "Vertex.h"
#include <string>

int problemCount;

uint32_t ReadFlowNetwork() {
    uint16_t order, size, source, sink;

    std::cin >> order;
    std::cin >> size;
    std::cin >> source;
    std::cin >> sink;

    //std::cout << "Graph with " << order << " vertices, " << size << " edges, source " << source
              //  << " and sink " << sink << "\n";

    Graph g(order);
    uint16_t v, u;
    uint32_t c;
    for (int i = 1; i <= size; i++) {
        std::cin >> v;
        std::cin >> u;
        std::cin >> c;
        //std::cout << "Added edge " << v << " to, " << u << " with capcity " << c << "\n";
        g.AddEdge(v, u, c);
    }
    return g.EdmondKarp(source, sink);
}

int main()
{
    std::cin >> problemCount;

    for (int i = 0; i < problemCount; i++) {
        std::cout << ReadFlowNetwork() << std::endl;
    }

    return 0;
}