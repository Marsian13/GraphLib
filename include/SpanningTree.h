/*
 * SpanningTree.h
 * Minimum spanning tree algorithms for weighted graphs.
 *
 * Includes:
 * - Union-Find (Disjoint Set Union)
 * - Kruskal's algorithm
 * - Prim's algorithm
 *
 * Design:
 * - Works with the templated Graph class.
 * - MST algorithms assume an undirected, connected graph.
 */

#pragma once
#include "Graph.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <queue>
#include <stdexcept>
#include <numeric>

namespace graphlib {

// ---- Union-Find (Disjoint Set Union) ----
template <typename T>
class UnionFind {
public:
    // Initializes each node as its own set.
    explicit UnionFind(const std::vector<T>& nodes) {
        for (const T& n : nodes) {
            parent_[n] = n;   // each node is initially its own parent
            rank_[n]   = 0;   // initial rank of every set
        }
    }

    // Returns the representative of x's set.
    // Applies path compression for faster future queries.
    T find(const T& x) {
        if (parent_[x] != x)
            parent_[x] = find(parent_[x]); // path compression

        return parent_[x];
    }

    // Merges the sets containing x and y.
    // Returns false if they are already in the same set.
    // Uses union by rank to keep trees shallow.
    bool unite(const T& x, const T& y) {
        T rx = find(x);
        T ry = find(y);
        if (rx == ry) return false; // already same component

        // Attach smaller rank tree under larger rank tree
        if (rank_[rx] < rank_[ry])  std::swap(rx, ry);

        parent_[ry] = rx;
        if (rank_[rx] == rank_[ry]) ++rank_[rx]; // Equal ranks: increase the new root's rank.

        return true;
    }

    // Returns true if x and y belong to the same set.
    bool connected(const T& x, const T& y) {
        return find(x) == find(y);
    }

private:
    std::unordered_map<T, T> parent_; // Parent of each node in the DSU forest.
    std::unordered_map<T, int> rank_; // Approximate tree height used for balancing.
};

//  ------ MSTResult — edges in the MST and total weight ------
template <typename T, typename W>
struct MSTResult {
    std::vector<std::pair<T, T>> edges; // (from, to) pairs in MST
    W totalWeight = W{0};
};

// ---- Kruskal's Algorithm ----
// Builds a minimum spanning tree using Union-Find.
// Time: O(E log E), Space: O(V + E)
template <typename T, typename W = int>
MSTResult<T, W> kruskal(const Graph<T, W>& graph) {
    // Gather all edges into a flat, sortable list
    struct FlatEdge { T u, v; W weight; };
    std::vector<FlatEdge> edges;

    for (const auto& [node, adj] : graph.adjacency())
        for (const auto& e : adj)
            // Skip the reverse copy of each undirected edge.
            if (node < e.to || graph.isDirected()) // avoid duplicates for undirected
                edges.push_back({node, e.to, e.weight});

    // Sort edges by ascending weight — the greedy choice
    std::sort(edges.begin(), edges.end(), [](const FlatEdge& a, const FlatEdge& b){
                  return a.weight < b.weight;
              });

    UnionFind<T> uf(graph.nodes());
    MSTResult<T, W> result;

    int edgesAdded = 0;
    int target = graph.nodeCount() - 1; // MST has exactly V-1 edges

    for (const auto& [u, v, w] : edges) {
        if (edgesAdded == target) break; // MST complete

        // Add the edge only if it doesn't create a cycle.
        if (uf.unite(u, v)) {
            result.edges.push_back({u, v});
            result.totalWeight += w;
            
            ++edgesAdded;
        }
    }

    if (edgesAdded < target)
        throw std::runtime_error("kruskal: graph is not connected — no MST exists");

    return result;
}

// ---- Prim's Algorithm ----
// Builds a minimum spanning tree by expanding from a start node.
// Similar to Dijkstra, but prioritizes edge weights.
// Time: O((V + E) log V), Space: O(V)
template <typename T, typename W = int>
MSTResult<T, W> prim(const Graph<T, W>& graph, const T& start) {
    using PQItem = std::tuple<W, T, T>; // (weight, to, from)
    std::priority_queue<PQItem, std::vector<PQItem>, std::greater<PQItem>> pq;

    std::unordered_set<T> inMST; // nodes already in the tree
    MSTResult<T, W> result;

    // Seed: push all edges from start with a dummy "from"
    inMST.insert(start);
    for (const auto& e : graph.neighbours(start))
        pq.push({e.weight, e.to, start});

    while (!pq.empty() && static_cast<int>(inMST.size()) < graph.nodeCount()) {
        auto [w, u, from] = pq.top();
        pq.pop();

        if (inMST.count(u)) continue; // already in tree, skip

        // Accept this edge — u joins the MST
        inMST.insert(u);
        result.edges.push_back({from, u});
        result.totalWeight += w;

        // Push all edges from u to nodes not yet in MST
        for (const auto& e : graph.neighbours(u))
            if (!inMST.count(e.to))
                pq.push({e.weight, e.to, u});
    }

    if (static_cast<int>(inMST.size()) < graph.nodeCount())
        throw std::runtime_error("prim: graph is not connected — no MST exists");

    return result;
}

} // namespace graphlib
