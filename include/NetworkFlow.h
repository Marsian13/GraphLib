/*
 * NetworkFlow.h
 * Maximum flow algorithms using the Ford-Fulkerson method.
 *
 * Includes:
 * - FlowNetwork (residual graph representation)
 * - Edmonds-Karp algorithm
 *
 * Design:
 * - Stores a residual graph to keep track of remaining capacities.
 * - Uses BFS (Edmonds-Karp) to find augmenting paths.
 * - Reverse edges allow flow to be redirected when needed.
 */

#pragma once
#include "Graph.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <stdexcept>

namespace graphlib {

// Returns a large value to represent infinite capacity.
template <typename W>
inline W flowInf() { return std::numeric_limits<W>::max() / 2; }

// ---- Residual Flow Network ----
// Maintains the residual graph used by max-flow algorithms.
template <typename T, typename W = int>
class FlowNetwork {
public:
    // Builds the residual graph from the given capacity graph.
    // Each edge gets a reverse edge with zero initial capacity.
    explicit FlowNetwork(const Graph<T, W>& capacityGraph) {
        for (const T& node : capacityGraph.nodes())
            residual_[node]; // Ensure every node exists.

        for (const auto& [u, adj] : capacityGraph.adjacency()) {
            for (const auto& e : adj) {
                // Store the forward edge capacity.
                residual_[u][e.to] += e.weight;

                // Reverse edge starts with zero capacity.
                residual_[e.to][u] += W{0};
            }
        }
    }

    // Finds an augmenting path using BFS.
    // Only edges with positive remaining capacity are visited.
    std::unordered_map<T, T> bfsFindPath(const T& s, const T& t) {
        std::unordered_map<T, T> parent;
        std::unordered_set<T> visited;
        std::queue<T> q;

        q.push(s);
        visited.insert(s);

        while (!q.empty()) {
            T u = q.front();
            q.pop();

            if (u == t)
                break; // Sink reached.

            for (const auto& [v, cap] : residual_[u]) {
                if (!visited.count(v) && cap > W{0}) {
                    visited.insert(v);
                    parent[v] = u;
                    q.push(v);
                }
            }
        }

        return parent; // Empty if no path exists.
    }

    // Returns the remaining capacity of an edge.
    W remainingCapacity(const T& u, const T& v) const {
        auto it = residual_.find(u);
        if (it == residual_.end()) return W{0};

        auto eit = it->second.find(v);
        return eit == it->second.end() ? W{0} : eit->second;
    }

    // Pushes flow through an edge and updates the reverse edge.
    void addFlow(const T& u, const T& v, W flow) {
        residual_[u][v] -= flow;
        residual_[v][u] += flow;
    }

private:
    // node -> (neighbour -> remaining capacity)
    std::unordered_map<T, std::unordered_map<T, W>> residual_;
};

// ---- Edmonds-Karp Maximum Flow ----
// Computes the maximum flow from source to sink.
// Time: O(V * E²), Space: O(V + E)
template <typename T, typename W = int>
W edmondsKarp(const Graph<T, W>& graph, const T& source, const T& sink) {
    FlowNetwork<T, W> network(graph);
    W maxFlow = W{0};

    while (true) {
        // Find an augmenting path.
        auto parent = network.bfsFindPath(source, sink);

        if (!parent.count(sink) && source != sink)
            break; // No augmenting path remains.

        // Find the bottleneck (minimum residual capacity)
        // along the current augmenting path.
        W pathFlow = flowInf<W>();
        T v = sink;
        
        while (v != source) {
            T u = parent.at(v);
            pathFlow = std::min(pathFlow, network.remainingCapacity(u, v));
            v = u;
        }

        // Update the residual graph after sending flow.
        v = sink;
        while (v != source) {
            T u = parent.at(v);
            network.addFlow(u, v, pathFlow);
            v = u;
        }

        maxFlow += pathFlow;
    }

    return maxFlow;
}

} // namespace graphlib