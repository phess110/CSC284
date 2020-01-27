#pragma once
#include <cstdint>
#include <vector>

using std::vector;

class Vertex
{
public:
    Vertex(uint16_t);
    Vertex();

    void AddNeighbor(uint16_t);
    vector<uint16_t>::iterator FindNeighbor(uint16_t idx);
    vector<uint16_t>::iterator EndNeighbor();
    void RemoveNeighbor(std::vector<uint16_t>::iterator);
    vector<uint16_t> Neighbors();

private:

    uint16_t index;
    vector<uint16_t> neighbors; 
};

