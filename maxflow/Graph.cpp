/**
    Implementation of Graph class for max flow algorithms
**/

#include "Graph.h"

Graph::Graph(uint16_t numVerts) :
    order(numVerts)
{
    for (uint16_t i = 1; i <= numVerts; i++) {
        vertices[i] = Vertex(i);
    }
};

/* Add additional vertex after construction */
void Graph::AddVertex() {
    order += 1;
    vertices[order] = Vertex(order);
}

/*
// Inserts a directed edge from head to tail with given capacity
// If there is already an edge from tail to head, insert intermediate vertex, i.e.
// Avoids antiparallel edges
// Should be used when constructing the initial flow problem
*/
void Graph::AddEdgeNoReversal(uint16_t head, uint16_t tail, uint32_t cap = 0) {
    list<uint16_t>* neighbors = vertices[tail].NeighborsPtr();
    if (std::find(neighbors->begin(), neighbors->end(), head) != neighbors->end()) {
        AddVertex();
        vertices[head].AddNeighbor(order);
        vertices[order].AddNeighbor(tail);
        capacity[edge(head, order)] = cap;
        capacity[edge(order, tail)] = cap;
    }
    else {
        neighbors = vertices[head].NeighborsPtr();
        if (std::find(neighbors->begin(), neighbors->end(), tail) != neighbors->end()) {
            capacity[edge(head, tail)] += cap;
        }
        else {
            vertices[head].AddNeighbor(tail);
            capacity[edge(head, tail)] = cap;
        }
    }
}

// Insert an the given edge into the graph with capacity
void Graph::AddEdge(uint16_t head, uint16_t tail, uint32_t cap = 0) {
    vertices[head].AddNeighbor(tail);
    capacity[edge(head, tail)] = cap;
}

// Ford-Fulkerson algorithm augmenting via shortest path
uint32_t Graph::EdmondKarp(uint16_t source, uint16_t sink) {
    // Initialize flow to 0
    signed_function f;
    for (uint16_t v = 1; v <= order; v++) {
        list<uint16_t> neighbors = vertices[v].Neighbors();
        list<uint16_t>::iterator it;
        for (it = neighbors.begin(); it != neighbors.end(); it++) {
            f[edge(v, *it)] = 0;
            f[edge(*it, v)] = 0;
        }
    }

    Graph residual = ResidualNetwork(f);
    vector<uint16_t> augPath;
    uint32_t minCap, val = 0;
    std::tie(augPath, minCap) = residual.ShortestPath(source, sink);

    while (augPath.size()) {
        for (uint16_t i = 0; i < augPath.size() - 1; i++) {
            list<uint16_t> neighs = vertices[augPath[i]].Neighbors();
            if (std::find(neighs.begin(), neighs.end(), augPath[i+1]) != neighs.end()) {
                f[edge(augPath[i], augPath[i + 1])] += minCap;
            }
            else {
                f[edge(augPath[i+1], augPath[i])] -= minCap;
            }
        }
        val += minCap;
        residual.AugmentResidualNetwork(this, f, augPath);
        std::tie(augPath, minCap) = residual.ShortestPath(source, sink);
    }

    return val;
}

// Returns the residual network associated with the given digraph and flow
Graph Graph::ResidualNetwork(signed_function flow) {
    // Duplicate current graph
    Graph gf(order);
    // set capacities - add an edge iff capacity(u,v) > flow(u,v)
    int32_t f, newcap;

    for (uint16_t v = 1; v <= order; v++) {
        list<uint16_t> neighbors = vertices[v].Neighbors();
        list<uint16_t>::iterator it;
        for (it = neighbors.begin(); it != neighbors.end(); it++) {
            uint16_t u = *it;
            f = flow[edge(v, u)];
            newcap = capacity[edge(v, u)] - f;

            if (newcap > 0) {
                gf.AddEdge(v, u, newcap);
            }

            newcap = f;
            if (f > 0) {
                gf.AddEdge(u, v, newcap);  
            }
        }
    }
    return gf;
}

// Adjust residual capacities along a given path
void Graph::AugmentResidualNetwork(Graph* g, signed_function& flow, vector<uint16_t> path) {
    uint32_t f, newcap;
    size_t len = path.size() - 1;
    uint16_t u, v, t;

    for (uint16_t i = 0; i < len; i++) {
        u = path[i];
        v = path[i + 1];
        list<uint16_t> neighbors = g->vertices[u].Neighbors();
        list<uint16_t>::iterator it = std::find(neighbors.begin(), neighbors.end(), v);

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

/*  
    Returns a vector consisting of the shortest start, end-path
    and the bottleneck capacity of that path.
*/
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

        list<uint16_t> neighbors = vertices[currIndex].Neighbors();
        list<uint16_t>::iterator it = neighbors.begin();
        for (it; it != neighbors.end(); it++) {
            uint16_t u = *it;
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

uint32_t Graph::PushRelabelFlow(uint16_t source, uint16_t sink) {
    signed_function f;     // flow
    vector<int> l;  // height labeling
    vector<uint32_t> excess;                // keep track of excess at each vertex
    l.push_back(0), excess.push_back(0);    // skip 1st entry (vertices are 1-indexed)
    std::list<uint16_t> active;             // list of currently active vertices

    // initialize preflow and valid labelling
    list<uint16_t>* neighbors;
    list<uint16_t>::iterator it;
    for (uint16_t v = 1; v <= order; v++) {
        neighbors = vertices[v].NeighborsPtr();
        it = neighbors->begin();
        l.push_back(0);
        for (it; it != neighbors->end(); it++) {
            f[edge(v, *it)] = 0;
            f[edge(*it, v)] = 0;
        }
        excess.push_back(0);
    }

    neighbors = vertices[source].NeighborsPtr();
    it = neighbors->begin();
    l[source] = order;
    uint32_t cap;
    for (it; it != neighbors->end(); it++) {
        cap = capacity[edge(source, *it)];
        f[edge(source, *it)] = cap;
        f[edge(*it, source)] = -(static_cast<int32_t>(cap));
        if (*it != sink) {
            active.push_back(*it);
        }
        excess[*it] = cap;
    }

    Graph residual = ResidualNetwork(f);

    uint16_t u;
    while (!active.empty()) {
        // grab active vertex with maximum height
        std::list<uint16_t>::iterator it = active.begin();
        std::list<uint16_t>::iterator loc = it;
        int m = l[*it];
        for (it; it != active.end(); it++) {
            if (l[*it] > m) {
                loc = it;
                m = l[*it];
            }
        }
        u = *loc;

        bool edgeUtoV, edgeVtoU, pushed = false;
        neighbors = vertices[u].NeighborsPtr();
        list<uint16_t> *vNeighbors;
        list<uint16_t> *residualNeighbors = residual.vertices[u].NeighborsPtr();
        // PUSH operation
        for (it = residualNeighbors->begin(); it != residualNeighbors->end(); it++) {
            if (l[u] != 1 + l[*it]) { continue; }
            // is the an edge from u to v in G
            edgeUtoV = (std::find(neighbors->begin(), neighbors->end(), *it) != neighbors->end());
            vNeighbors = residual.vertices[*it].NeighborsPtr();
            edgeVtoU = (std::find(vNeighbors->begin(), vNeighbors->end(), u) != vNeighbors->end());
            pushed = true;

            uint32_t delta;
            if (edgeUtoV) {
                delta = std::min(excess[u], capacity[edge(u, *it)] - f[edge(u, *it)]);
                // update residual
                if (f[edge(u, *it)] == 0 && !edgeVtoU) {
                    residual.AddEdge(*it, u, delta);
                }
                else {
                    residual.capacity[edge(*it, u)] += delta;
                }
            }
            else {
                delta = std::min(excess[u], (uint32_t) f[edge(*it, u)]);
                // update residual
                if (f[edge(*it, u)] == capacity[edge(*it, u)] && !edgeVtoU) {
                    residual.AddEdge(*it, u, delta);
                }
                else {
                    residual.capacity[edge(*it, u)] += delta;
                }
            }

            // update flow
            f[edge(u, *it)] += delta;
            f[edge(*it, u)] -= delta;

            // did v become newly active
            if (excess[*it] == 0 && *it != sink && *it != source) {
                active.push_back(*it);
            }

            // update excesses
            excess[u] -= delta;
            excess[*it] += delta;

            // update residual graph for edge (u,v)
            residual.capacity[edge(u, *it)] -= delta;
            if (residual.capacity[edge(u, *it)] == 0) {
                residual.vertices[u].RemoveNeighbor(it);
            }

            // u inactive?
            if (excess[u] == 0) {
                active.erase(loc); // remove u
            }

            break;
        }
        // RELABEL
        if (!pushed) {
            Relabel(u, residual, l);
        }
    }

    return excess[sink];
}

// Modify height labeling so that l[u] = 1 + min_{(u,v) in Gf} l[v]
void Graph::Relabel(uint16_t u, Graph& residual, vector<int> &l) {

    int m = INT32_MAX;
    list<uint16_t>* neighbors = residual.vertices[u].NeighborsPtr();
    list<uint16_t>::iterator it = neighbors->begin();
    for (it; it != neighbors->end(); it++) {
        m = std::min(m, l[*it]);
    }

    l[u] = m + 1;
}
