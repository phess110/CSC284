#include "Graph.h"

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
    signed_function f;
    // TODO perhaps this should be int32_t not uint32_t?
    // TODO make flow symmetric for simplicity?
    // maybe there are loops causing problems?

    for (uint16_t v = 1; v <= order; v++) {
        vector<uint16_t> neighbors = vertices[v].Neighbors();
        for (std::size_t i = 0; i < neighbors.size(); i++) {
            f[edge(v, neighbors[i])] = 0;
            f[edge(neighbors[i], v)] = 0;
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
// Returns the residual network associated with the given digraph and flow
Graph Graph::ResidualNetwork(signed_function flow) {
    // Duplicate current graph
    Graph gf(order);
    // set capacities - add an edge iff capacity(u,v) > flow(u,v)
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

void Graph::AugmentResidualNetwork(Graph* g, signed_function flow, vector<uint16_t> path) {
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

uint32_t Graph::PushRelabelFlow(uint16_t source, uint16_t sink) {
    signed_function f;     // flow
    vector<int> l;  // height labeling
    vector<uint32_t> excess;                // keep track of excess at each vertex
    l.push_back(0), excess.push_back(0);    // skip 1st entry (vertices are 1-indexed)
    std::set<uint16_t> active;              // list of currently active vertices

    // initialize preflow and valid labelling
    vector<uint16_t> neighbors;
    for (uint16_t v = 1; v <= order; v++) {
        neighbors = vertices[v].Neighbors();
        l.push_back(0);
        for (std::size_t i = 0; i < neighbors.size(); i++) {
            f[edge(v, neighbors[i])] = 0;
            f[edge(neighbors[i], v)] = 0;
        }
        excess.push_back(0);
    }

    neighbors = vertices[source].Neighbors();
    l[source] = order;
    uint32_t cap;
    for (std::size_t i = 0; i < neighbors.size(); i++) {
        cap = capacity[edge(source, neighbors[i])];
        f[edge(source, neighbors[i])] = cap;
        f[edge(neighbors[i], source)] = -static_cast<int32_t>(cap);
        active.insert(neighbors[i]);
        excess[neighbors[i]] = cap;
    }

    uint16_t u;
    while (!active.empty()) {
        // grab active vertex with maximum height
        std::set<uint16_t>::iterator it = active.begin();
        u = *it;
        int m = l[*it];
        for (it; it != active.end(); it++) {
            if (l[*it] > m) {
                u = *it;
                m = l[*it];
            }
        }

        // use sentinel
        bool pushed = false;
        for (uint16_t v = 1; v <= order; v++) {
            if (l[u] != 1 + l[v]) { continue; }
            if (capacity[edge(u, v)] > f[edge(u, v)]) {
                // (u,v) in Gf
                pushed = true;
                Push(u, v, f, active, excess);
                break;
            }
        }

        if (!pushed) {
            // call relabel of u
            Relabel(u, f, l);
        }
    }

    uint32_t val = 0;

    return val;
}

void Graph::Push(uint16_t u,
                uint16_t v, 
                signed_function& flow,
                std::set<uint16_t>& active, 
                vector<uint32_t>& excess) {

    uint32_t excess_u = excess[u];
    // is v a neighbor of u
    //      increase f[u,v] by min(a_u, c(u,v) - f[u,v])
    //      decrease f[v,u] by same amount
    // otherwise u is a neighbor of v
    //      increase = min(a_u, f[u,v])
    //      decrease f[v,u] by same amount

    vector<uint16_t> neighbors = vertices[u].Neighbors();
    uint32_t delta;
    if (std::find(neighbors.begin(), neighbors.end(), v) != neighbors.end()) {
        delta = std::min(excess_u, capacity[edge(u,v)] - flow[edge(u,v)]);
        flow[edge(u, v)] += delta;
        flow[edge(v, u)] -= delta;
    }
    else {
        delta = std::min(excess_u, flow[edge(u, v)]);
        flow[edge(u, v)] -= delta;
        flow[edge(v, u)] += delta;
    }

    // update excesses
    excess[u] -= delta;
    excess[v] += delta;

    // track which vertices become active/inactive.
    if (delta == excess_u) {
        active.erase(u);
    }
    active.insert(v);
}

// l[u] = min_{(u,v) in Gf} l[v] + 1
void Graph::Relabel(uint16_t u, signed_function &flow, vector<int> &l) {
    vector<uint16_t> neighbors = vertices[u].Neighbors();
    int m = INT32_MAX;
    for (size_t i = 0; i < neighbors.size(); i++) {
        if (capacity[edge(u, neighbors[i])] > flow[edge(u,neighbors[i])]) {
            m = std::min(m, l[neighbors[i]]);
        }
    }

    for (uint16_t v = 0; v < order; v++) {
        if (v == u) { continue; }
        neighbors = vertices[v].Neighbors();
        if (std::find(neighbors.begin(), neighbors.end(), u) != neighbors.end()) {
            // u is a neighbor of v
            if (flow[edge(v,u)] > 0) {
                m = std::min(m, l[v]);
            }
        }
    }
    // for v in V, if u is a neighbor of V
    //  if f[v,u] > 0
    //        l[u] = min()

    l[u] = m + 1;
}
