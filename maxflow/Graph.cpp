#include "Graph.h"
#include <queue>

Graph::Graph(uint16_t numVerts) :
    order(numVerts)
{
    for (uint16_t i = 1; i <= numVerts; i++) {
        vertices[i] = Vertex(i);
    }
};

void Graph::AddEdge(uint16_t head, uint16_t tail, uint32_t cap = 0) {
    vertices[head].AddNeighbor(tail);
    capacity[edge(head, tail)] = cap;
}

// Ford-Fulkerson algorithm
uint32_t Graph::EdmondKarp(uint16_t source, uint16_t sink) {

    // Initialize flow to 0
    function f;

    for (uint16_t v = 1; v <= order; v++) {
        vector<uint16_t> neighbors = vertices[v].Neighbors();
        for (std::size_t i = 0; i < neighbors.size(); i++) {
            f[edge(v, neighbors[i])] = 0;
        }
    }

    Graph residual = ResidualNetwork(f);
    vector<uint16_t> augPath;
    uint32_t minCap, val = 0;
    std::tie(augPath, minCap) = residual.ShortestPath(source, sink);

    while (augPath.size()) {
        for (uint16_t i = 0; i < augPath.size() - 1; i++) {
            vector<uint16_t> neighs = vertices[augPath[i]].Neighbors();
            if (std::find(neighs.begin(), neighs.end(), augPath[i+1]) != neighs.end()) {
                f[edge(augPath[i], augPath[i + 1])] += minCap;
            }
            else {
                f[edge(augPath[i+1], augPath[i])] -= minCap;
            }
        }
        val += minCap;
        //residual = ResidualNetwork(f);
        residual.AugmentResidualNetwork(this, f, augPath);
        std::tie(augPath, minCap) = residual.ShortestPath(source, sink);
    }

    return val;
}

// O(E)
Graph Graph::ResidualNetwork(function flow) {
    // Duplicate current graph
    Graph gf(order);
   
    // set capacities - adding an edge iff capacity(u,v) > flow(u,v)
    uint32_t f, newcap;

    for (uint16_t v = 1; v <= order; v++) {
        vector<uint16_t> neighbors = vertices[v].Neighbors();
        for (std::size_t i = 0; i < neighbors.size(); i++) {
            uint16_t u = neighbors[i];
            f = flow[edge(v, u)];
            newcap = capacity[edge(v, u)] - f;

            if (newcap > 0) {
                gf.AddEdge(v, u, newcap);
            }

            newcap = f + (capacity.count(edge(u, v)) ? capacity[edge(u,v)] : 0);
        
            if (newcap > 0) {
                gf.AddEdge(u, v, newcap);  
            }
        }
    }

    return gf;
}

void Graph::AugmentResidualNetwork(Graph* g, function flow, vector<uint16_t> path) {
    uint32_t f, newcap;
    uint16_t len = path.size() - 1;
    uint16_t u, v, t;

    for (uint16_t i = 0; i < len; i++) {
        u = path[i];
        v = path[i + 1];
        vector<uint16_t> neighbors = g->vertices[u].Neighbors();
        vector<uint16_t>::iterator it = std::find(neighbors.begin(), neighbors.end(), v);

        // If u -> v is not in G, switch
        if (it == neighbors.end()) {
            t = v;
            v = u;
            u = t;
        }

        f = flow[edge(u, v)];
        newcap = g->capacity[edge(u, v)] - f;
        it = vertices[u].FindNeighbor(v);

        // Insert edge if capacity > 0, otherwise remove it.
        if (newcap) {
            // u -> v not in Gf
            if (it == vertices[u].EndNeighbor()) {
                AddEdge(u, v, newcap);
            }
            else {
                capacity[edge(u, v)] = newcap;
            }
        }
        else if (it != vertices[u].EndNeighbor()){
            // remove edge
           vertices[u].RemoveNeighbor(it);
        }

        // set capacity of v,u to f.
        it = vertices[v].FindNeighbor(u);
        if (f) {
            if (it == vertices[v].EndNeighbor()) {
                AddEdge(v, u, f);
            }
            else {
                capacity[edge(v, u)] = f;
            }
        }
        else if (it != vertices[v].EndNeighbor()) {
            vertices[v].RemoveNeighbor(it);
        }
    }
}

pair<vector<uint16_t>, uint32_t> Graph::ShortestPath(uint16_t start, uint16_t end) {
    queue<uint16_t> queue;
    unordered_map<uint16_t, bool> visited;
    vector<uint16_t> parents; // change to map

    // set visited to false
    for (uint16_t i = 0; i <= order; i++) {
        visited[i] = false;
        parents.push_back(i);
    }

    queue.push(start);
    visited[start] = true;
    uint16_t currIndex;
    while (!queue.empty()) {
        currIndex = queue.front();
        queue.pop();

        Vertex curr = vertices[currIndex];
        for (size_t i = 0; i < curr.Neighbors().size(); i++) {
            uint16_t u = curr.Neighbors()[i];
            if (!visited[u]) {
                queue.push(u);
                parents[u] = currIndex;
                visited[u] = true;
            }
        }
    }

    // backtrack from end, return empty path if unreachable
    vector<uint16_t> path;
    currIndex = end;
    uint32_t minCapacity;
    if (visited[end]) {
        path.push_back(end);
        minCapacity = capacity[edge(parents[end], end)];
        while (currIndex != start) {
            minCapacity = std::min(minCapacity, capacity[edge(parents[currIndex], currIndex)]);
            currIndex = parents[currIndex];
            path.push_back(currIndex);
        }

        std::reverse(path.begin(), path.end());
    }

    return pair<vector<uint16_t>, uint32_t>(path, minCapacity);
}

typedef pair<uint16_t, uint32_t> vertExcessPair;
auto cmp = [](vertExcessPair u, vertExcessPair v)
    { return u.second >= v.second; };
typedef priority_queue<vertExcessPair, vector<vertExcessPair>, decltype(cmp)> excessQueue;

uint32_t Graph::PushRelabelFlow(uint16_t source, uint16_t sink) {
    function f; // flow
    vector<int> l;
    l.push_back(0); // ignore this first entry

    // store pairs (vertex, excess) with non-zero excess, ordered by excess
    excessQueue excess(cmp);
     
    // initialize preflow and valid labelling
    vector<uint16_t> neighbors;
    for (uint16_t v = 1; v <= order; v++) {
        neighbors = vertices[v].Neighbors();
        l.push_back(0);
        for (std::size_t i = 0; i < neighbors.size(); i++) {
            f[edge(v, neighbors[i])] = 0;
        }
    }

    neighbors = vertices[source].Neighbors();
    l[source] = order;
    for (std::size_t i = 0; i < neighbors.size(); i++) {
        f[edge(source, neighbors[i])] = capacity[edge(source, neighbors[i])];
        excess.push(vertExcessPair(neighbors[i], capacity[edge(source, neighbors[i])]));
    }

    uint16_t u;
    while (!excess.empty()) {
        u = excess.top().first;

        for (uint16_t v = 1; v <= order; v++) {
            if (l[u] != 1 + l[v]) { continue; }
            if (capacity[edge(u, v)] > f[edge(u, v)]) {

            }
        }

        if (true) {

        }
        else {

        }
    }

    uint32_t val = 0;

    return val;
}

void Push(uint16_t u, uint16_t v, function &flow, excessQueue& excess) {


    // also update excesses here!
}

void Relabel(uint16_t u, vector<uint16_t, int> &l) {
    
}
