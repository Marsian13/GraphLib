/*
 * Connectivity.h
 * Connectivity algorithms for directed and undirected graphs.
 *
 * Includes:
 * - Tarjan's Strongly Connected Components
 * - Kosaraju's Strongly Connected Components
 * - Bridge detection
 * - Articulation point detection
 *
 * Design:
 * - Tarjan's algorithm finds SCCs in a single DFS.
 * - Kosaraju's algorithm uses two DFS passes and a transpose graph.
 * - Bridges and articulation points are found using DFS discovery
 *   and low-link values.
 */

#pragma once
#include "Graph.h"
#include <vector>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <functional>

namespace graphlib {

// ---- Tarjan's Strongly Connected Components ----
// Finds all strongly connected components in a directed graph.
// Uses a single DFS traversal.
// Time: O(V + E), Space: O(V)
template <typename T, typename W = int>
std::vector<std::vector<T>> tarjanSCC(const Graph<T, W>& graph) {
    std::unordered_map<T, int> disc, low;
    std::unordered_set<T> onStack;
    std::stack<T> stk;
    std::vector<std::vector<T>> sccs;
    int timer = 0;

    // -1 indicates that the node has not been visited.
    for (const T& node : graph.nodes()) disc[node] = -1;

    std::function<void(const T&)> dfsVisit = [&](const T& u) {
        disc[u] = low[u] = timer++;
        stk.push(u);
        onStack.insert(u);

        for (const auto& edge : graph.neighbours(u)) {
            const T& v = edge.to;

            if (disc[v] == -1) {
                // Explore the subtree and update the low-link value.
                dfsVisit(v);

                low[u] = std::min(low[u], low[v]);
            } else if (onStack.count(v)) {
                // Update the low-link value using a back edge.
                // Use disc[v] instead of low[v] to avoid merging
                // different strongly connected components.
                low[u] = std::min(low[u], disc[v]);
            }
            // Ignore nodes that already belong to a completed SCC.
        }

        // If u is the root of an SCC, pop all nodes in that component.
        if (low[u] == disc[u]) {
            std::vector<T> component;
            T w;

            do {
                w = stk.top(); stk.pop();
                onStack.erase(w);
                component.push_back(w);
            } while (w != u);

            sccs.push_back(component);
        }
    };

    for (const T& node : graph.nodes())
        if (disc[node] == -1)
            dfsVisit(node);

    return sccs;
}

// ---- Graph Transpose ----
// Returns a new graph with every edge reversed.
// Used by Kosaraju's algorithm.
template <typename T, typename W = int>
Graph<T, W> transpose(const Graph<T, W>& graph) {
    Graph<T, W> rev(true); // Transpose is always directed.

    for (const T& node : graph.nodes())
        rev.addNode(node);

    for (const auto& [u, adj] : graph.adjacency())
        for (const auto& e : adj)
            rev.addEdge(e.to, u, e.weight); // Reverse the edge.

    return rev;
}

// ---- Kosaraju's Strongly Connected Components ----
// Finds all strongly connected components using two DFS passes.
// Time: O(V + E), Space: O(V)
template <typename T, typename W = int>
std::vector<std::vector<T>> kosarajuSCC(const Graph<T, W>& graph) {
    // First DFS: record nodes in finishing order.
    std::unordered_set<T> visited;
    std::stack<T> finishOrder;

    std::function<void(const T&)> dfs1 = [&](const T& u) {
        visited.insert(u);

        for (const auto& edge : graph.neighbours(u))
            if (!visited.count(edge.to))
                dfs1(edge.to);
                
        finishOrder.push(u); // Node finished.
    };

    for (const T& node : graph.nodes())
        if (!visited.count(node))
            dfs1(node);

    // Reverse all graph edges.
    Graph<T, W> rev = transpose(graph);

    // Second DFS on the transpose graph.
    visited.clear();
    std::vector<std::vector<T>> sccs;

    std::function<void(const T&, std::vector<T>&)> dfs2 =
        [&](const T& u, std::vector<T>& component) {

            visited.insert(u);
            component.push_back(u);

            for (const auto& edge : rev.neighbours(u))
                if (!visited.count(edge.to))
                    dfs2(edge.to, component);
        };

    while (!finishOrder.empty()) {
        T node = finishOrder.top();
        finishOrder.pop();

        if (!visited.count(node)) {
            std::vector<T> component;
            dfs2(node, component); // Collect one SCC.

            sccs.push_back(component);
        }
    }

    return sccs;
}

// ---- Bridge Detection ----
// Finds all bridges in an undirected graph.
// Time: O(V + E), Space: O(V)
template <typename T, typename W = int>
std::vector<std::pair<T, T>> findBridges(const Graph<T, W>& graph) {
    std::unordered_map<T, int> disc, low;
    std::unordered_map<T, T> parent;
    std::vector<std::pair<T, T>> bridges;
    int timer = 0;

    // -1 indicates that the node has not been visited.
    for (const T& node : graph.nodes()) disc[node] = -1;

    std::function<void(const T&)> dfsVisit = [&](const T& u) {
        disc[u] = low[u] = timer++;

        for (const auto& edge : graph.neighbours(u)) {
            const T& v = edge.to;

            if (disc[v] == -1) {
                parent[v] = u;
                dfsVisit(v);
                low[u] = std::min(low[u], low[v]);

                // If v cannot reach u or any ancestor of u,
                // then (u, v) is a bridge.
                if (low[v] > disc[u])
                    bridges.push_back({u, v});

            } else if (!parent.count(u) || v != parent[u]) {
                // Ignore the edge back to the parent and
                // update the low-link value using back edges.
                low[u] = std::min(low[u], disc[v]);
            }
        }
    };

    for (const T& node : graph.nodes())
        if (disc[node] == -1)
            dfsVisit(node);

    return bridges;
}

// ---- Articulation Point Detection ----
// Finds all articulation points in an undirected graph.
// Time: O(V + E), Space: O(V)
template <typename T, typename W = int>
std::vector<T> findArticulationPoints(const Graph<T, W>& graph) {
    std::unordered_map<T, int> disc, low;
    std::unordered_map<T, T> parent;
    std::unordered_set<T> isArticulation;
    int timer = 0;

    // -1 indicates that the node has not been visited.
    for (const T& node : graph.nodes()) disc[node] = -1;

    std::function<void(const T&, bool)> dfsVisit = [&](const T& u, bool isRoot) {
        disc[u] = low[u] = timer++;
        int children = 0; // Number of DFS children.

        for (const auto& edge : graph.neighbours(u)) {
            const T& v = edge.to;

            if (disc[v] == -1) {
                parent[v] = u;
                ++children;

                dfsVisit(v, false);
                low[u] = std::min(low[u], low[v]);

                // If v cannot reach an ancestor of u,
                // then u is an articulation point.
                if (!isRoot && low[v] >= disc[u])
                    isArticulation.insert(u);

            } else if (!parent.count(u) || v != parent[u]) {
                // Ignore the parent edge and update the low-link value.
                low[u] = std::min(low[u], disc[v]);
            }
        }

        // The DFS root is an articulation point only if
        // it has two or more DFS children.
        if (isRoot && children >= 2)
            isArticulation.insert(u);
            
    };

    for (const T& node : graph.nodes())
        if (disc[node] == -1)
            dfsVisit(node, true);

    return std::vector<T>(isArticulation.begin(), isArticulation.end());
}

} // namespace graphlib