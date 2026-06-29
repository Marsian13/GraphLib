/*
 * ShortestPath.h
 * Implementations of common shortest path algorithms.
 *
 * Includes:
 * - Dijkstra's algorithm (non-negative edge weights)
 * - Bellman-Ford algorithm (supports negative edge weights)
 * - Floyd-Warshall algorithm (all-pairs shortest paths)
 *
 * Design:
 * - Works with the templated Graph class.
 * - Returns result objects containing distances and
 *   helper functions for path reconstruction.
 */

#pragma once
#include "Graph.h"
#include <unordered_map>
#include <vector>
#include <queue>
#include <limits>
#include <stdexcept>
#include <algorithm>
#include <functional>

namespace graphlib {

template <typename W>
inline W INF() { return std::numeric_limits<W>::max() / 2; }

template <typename T, typename W = int>
struct ShortestPathResult {
    std::unordered_map<T, W> dist; // node -> distance from source
    std::unordered_map<T, T> prev; // node -> previous node in path

    std::vector<T> reconstructPath(const T& source, const T& target) const {
        std::vector<T> path;

        if (dist.count(target) == 0 || dist.at(target) == INF<W>())
            return path; // unreachable

        for (T at = target; at != source; at = prev.at(at))
            path.push_back(at);

        path.push_back(source);
        std::reverse(path.begin(), path.end());

        return path;
    }
};

// ---- Dijkstra's Algorithm ----
template <typename T, typename W = int>
ShortestPathResult<T, W> dijkstra(const Graph<T, W>& graph, const T& source) {
    ShortestPathResult<T, W> result;

    for (const T& node : graph.nodes()) {
        result.dist[node] = INF<W>();
    }
    result.dist[source] = W{0};

    using PQItem = std::pair<W, T>;
    std::priority_queue<PQItem, std::vector<PQItem>, std::greater<PQItem>> pq;
    pq.push({W{0}, source});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        if (d > result.dist[u]) continue;

        for (const auto& edge : graph.neighbours(u)) {
            W newDist = result.dist[u] + edge.weight;

            if (newDist < result.dist[edge.to]) {
                result.dist[edge.to] = newDist;
                result.prev[edge.to] = u;

                pq.push({newDist, edge.to});
            }
        }
    }

    return result;
}

// --- Bellman-Ford Algorithm ----
template <typename T, typename W = int>
std::pair<ShortestPathResult<T, W>, bool>
bellmanFord(const Graph<T, W>& graph, const T& source) {
    ShortestPathResult<T, W> result;

    for (const T& node : graph.nodes())
        result.dist[node] = INF<W>();
    result.dist[source] = W{0};

    int V = graph.nodeCount();

    struct FlatEdge { T from, to; W weight; };
    std::vector<FlatEdge> edges;
    for (const auto& [u, adj] : graph.adjacency())
        for (const auto& e : adj)
            edges.push_back({u, e.to, e.weight});

    for (int pass=0; pass<V-1; pass++) {
        bool updated = false;

        for (const auto& [u, v, w] : edges) {
            if (result.dist[u] != INF<W>() && result.dist[u] + w < result.dist[v]) {
                result.dist[v] = result.dist[u] + w;
                result.prev[v] = u;

                updated = true;
            }
        }

        if (!updated) break;
    }

    bool negCycle = false;
    for (const auto& [u, v, w] : edges) {
        if (result.dist[u] != INF<W>() && result.dist[u] + w < result.dist[v]) {
            negCycle = true;
            break;
        }
    }

    return {result, negCycle};
}

// ---- Floyd-Warshall Algorithm ----
template <typename T, typename W>
struct FloydWarshallResult {
    std::vector<std::vector<W>> dist;       // dist[i][j] = shortest i->j
    std::vector<T> nodes;                   // index -> node mapping
    std::unordered_map<T, int> nodeIndex;   // node -> index

    W getDistance(const T& from, const T& to) const {
        return dist[nodeIndex.at(from)][nodeIndex.at(to)];
    }

    bool hasNegativeCycle() const {
        for (int i=0; i<static_cast<int>(dist.size()); i++)
            if (dist[i][i] < W{0}) return true;

        return false;
    }
};


template <typename T, typename W = int>
FloydWarshallResult<T, W> floydWarshall(const Graph<T, W>& graph) {
    FloydWarshallResult<T, W> result;

    result.nodes = graph.nodes();
    int V = static_cast<int>(result.nodes.size());
    for (int i=0; i<V; i++)
        result.nodeIndex[result.nodes[i]] = i;

    result.dist.assign(V, std::vector<W>(V, INF<W>()));
    for (int i=0; i<V; i++) result.dist[i][i] = W{0};

    for (const auto& [u, adj] : graph.adjacency()) {
        int ui = result.nodeIndex[u];
        for (const auto& e : adj) {
            int vi = result.nodeIndex[e.to];
            result.dist[ui][vi] = std::min(result.dist[ui][vi], e.weight);
        }
    }

    for (int k=0; k<V; k++) {
        for (int i=0; i<V; i++) {
            for (int j=0; j<V; j++) {

                if (result.dist[i][k] != INF<W>() && result.dist[k][j] != INF<W>()) {
                    W through = result.dist[i][k] + result.dist[k][j];

                    if (through < result.dist[i][j])
                        result.dist[i][j] = through;
                }
            }
        }
    }
    return result;
}

} // graphlib