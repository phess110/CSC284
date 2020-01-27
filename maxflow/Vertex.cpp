#include "Vertex.h"

Vertex::Vertex() : index(0) {};

Vertex::Vertex(uint16_t i) :
    index(i)
{};

vector<uint16_t> Vertex::Neighbors() {
    return neighbors;
}

void Vertex::AddNeighbor(uint16_t idx) {
    neighbors.push_back(idx);
}

vector<uint16_t>::iterator Vertex::FindNeighbor(uint16_t idx) {
    return std::find(neighbors.begin(), neighbors.end(), idx);
}

vector<uint16_t>::iterator Vertex::EndNeighbor() {
    return neighbors.end();
}

void Vertex::RemoveNeighbor(vector<uint16_t>::iterator it) {
    neighbors.erase(it);
}