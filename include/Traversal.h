/*
 * Traversal.h
 * Common graph traversal algorithms.
 *
 * Includes:
 * - Breadth-First Search (BFS)
 * - Depth-First Search (DFS)
 * - Topological Sort
 * - Cycle Detection (Directed Graphs)
 * - Connected Components (Undirected Graphs)
 *
 * Design:
 * - Works with the templated Graph class.
 * - Uses iterative implementations where practical.
 */

#pragma once
#include "Graph.h"
#include <queue>
#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <functional>

namespace graphlib {

// ---- Breadth-First Search (BFS) ----
// Returns nodes in the order they are visited.
// Time: O(V + E), Space: O(V)
template <typename T, typename W = int>
std::vector<T> bfs(const Graph<T, W>& graph, const T& start) {
    std::vector<T> order;            // visit order (return value)
    std::unordered_set<T> visited;   // O(1) membership test
    std::queue<T> q;

    // Seed the queue with the start node
    visited.insert(start);
    q.push(start);

    while (!q.empty()) {
        T curr = q.front();
        q.pop();
        order.push_back(curr);

        // Visit each unvisited neighbour once.
        for (const auto& edge : graph.neighbours(curr)) {
            if (!visited.count(edge.to)) {
                visited.insert(edge.to);
                q.push(edge.to);
            }
        }
    }

    return order;
}

// ---- Depth-First Search (DFS) ----
// Iterative implementation using an explicit stack.
// Time: O(V + E), Space: O(V)
template <typename T, typename W = int>
std::vector<T> dfs(const Graph<T, W>& graph, const T& start) {
    std::vector<T> order;
    std::unordered_set<T> visited;
    std::stack<T> st;

    st.push(start);

    while (!st.empty()) {
        T curr = st.top();
        st.pop();

        if (visited.count(curr)) continue; // already processed
        visited.insert(curr);
        order.push_back(curr);

        /*
         *  Push neighbours in reverse so the leftmost neighbour
         *  is processed first (mimics recursive DFS behaviour).
         */
        const auto& nbrs = graph.neighbours(curr);
        for (auto it = nbrs.rbegin(); it != nbrs.rend(); it++) {
            if (!visited.count(it->to))
                st.push(it->to);
        }
    }
    
    return order;
}

// ---- DFS with Callback ----
// Performs DFS and invokes the callback for each visited node.
template <typename T, typename W = int>
void dfsCallback(const Graph<T, W>& graph,
                 const T& start,
                 const std::function<void(const T&)>& onVisit)
{
    std::unordered_set<T> visited;

    // Recursive lambda used for DFS.
    std::function<void(const T&)> recurse = [&](const T& node) {
        visited.insert(node);
        onVisit(node);

        for (const auto& edge : graph.neighbours(node))
            if (!visited.count(edge.to))
                recurse(edge.to);
    };

    recurse(start);
}

// ---- Cycle Detection ----
// Uses the DFS 3-color technique to detect cycles in a directed graph.
// Time: O(V + E), Space: O(V)
template <typename T, typename W = int>
bool hasCycle(const Graph<T, W>& graph) {
    // 0 = unvisited, 1 = in stack, 2 = done
    std::unordered_map<T, int> color;
    for (const T& node : graph.nodes())
        color[node] = 0;

    bool cycleFound = false;

    std::function<void(const T&)> dfsVisit = [&](const T& u) {
        if (cycleFound) return;
        color[u] = 1; // mark as being explored

        for (const auto& edge : graph.neighbours(u)) {
            if (color[edge.to] == 1) {  // back edge → cycle!
                cycleFound = true;
                return;
            }

            if (color[edge.to] == 0)
                dfsVisit(edge.to);
        }

        color[u] = 2; // fully explored
    };

    for (const T& node : graph.nodes())
        if (color[node] == 0)
            dfsVisit(node);

    return cycleFound;
}

// ---- Topological Sort ----
// Returns a topological ordering of a DAG.
// Throws if the graph contains a cycle.
// Time: O(V + E), Space: O(V)
template <typename T, typename W = int>
std::vector<T> topologicalSort(const Graph<T, W>& graph) {
    if (hasCycle(graph))
        throw std::runtime_error("topologicalSort: graph has a cycle");

    std::unordered_set<T> visited;
    std::stack<T> finishStack; // nodes pushed here when fully done

    std::function<void(const T&)> dfsVisit = [&](const T& u) {
        visited.insert(u);

        for (const auto& edge : graph.neighbours(u))
            if (!visited.count(edge.to))
                dfsVisit(edge.to);

        finishStack.push(u); // push AFTER all descendants are done
    };

    for (const T& node : graph.nodes())
        if (!visited.count(node))
            dfsVisit(node);

    // Pop stack into result vector → topological order
    std::vector<T> result;
    result.reserve(graph.nodeCount());

    while (!finishStack.empty()) {
        result.push_back(finishStack.top());
        finishStack.pop();
    }

    return result;
}

// ---- Connected Components ----
// Finds all connected components in an undirected graph.
// Time: O(V + E), Space: O(V)
template <typename T, typename W = int>
std::vector<std::vector<T>> connectedComponents(const Graph<T, W>& graph) {
    std::unordered_set<T> visited;
    std::vector<std::vector<T>> components;

    for (const T& node : graph.nodes()) {
        if (!visited.count(node)) {
            // BFS from this unvisited node → one new component
            std::vector<T> comp;
            std::queue<T> q;
            q.push(node);
            visited.insert(node);

            while (!q.empty()) {
                T curr = q.front();
                q.pop();
                comp.push_back(curr);

                for (const auto& e : graph.neighbours(curr))
                    if (!visited.count(e.to)) {
                        visited.insert(e.to);
                        q.push(e.to);
                    }
            }

            components.push_back(comp);
        }
    }

    return components;
}

} // namespace graphlib
