/**
    Implementation of Vertex class for max flow algorithms
**/

#include "Vertex.h"

Vertex::Vertex() : index(0) {};

Vertex::Vertex(uint16_t i) :
    index(i)
{};

list<uint16_t> Vertex::Neighbors() {
    return neighbors;
}

list<uint16_t>* Vertex::NeighborsPtr() {
    return &neighbors;
}

void Vertex::AddNeighbor(uint16_t idx) {
    neighbors.push_back(idx);
}

list<uint16_t>::iterator Vertex::FindNeighbor(uint16_t idx) {
    return std::find(neighbors.begin(), neighbors.end(), idx);
}

list<uint16_t>::iterator Vertex::EndNeighbor() {
    return neighbors.end();
}

void Vertex::RemoveNeighbor(list<uint16_t>::iterator it) {
    neighbors.erase(it);
}