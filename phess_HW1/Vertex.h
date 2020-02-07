#pragma once
#include <algorithm>
#include <cstdint>
#include <list>

using std::list;

class Vertex {
public:
    Vertex();

    void AddNeighbor(uint16_t);
    list<uint16_t>::iterator FindNeighbor(uint16_t idx);
    list<uint16_t>::iterator EndNeighbor();
    void RemoveNeighbor(list<uint16_t>::iterator);
    list<uint16_t> Neighbors();
    list<uint16_t> *NeighborsPtr();

private:
    list<uint16_t> neighbors; 
};

