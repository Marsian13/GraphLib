/*
 * Graph.h
 * Core templated adjacency-list graph representation.
 *
 * Design:
 * - Uses an adjacency list, making it suitable for sparse graphs.
 * - Uses std::unordered_map so nodes can be of any hashable type
 *   (e.g., int, std::string, or custom types).
 * - Not thread-safe. External synchronization is required for
 *   concurrent access.
 */

#pragma once
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <ostream>
#include <algorithm>

namespace graphlib {

// Represents a single outgoing edge from a node.
template <typename T, typename W = int>
struct Edge {
    T to;       // destination node
    W weight;   // edge weight (default 1 for unweighted graphs)

    Edge(T destination, W w = W{1}) : to(destination), weight(w) {}
};

template <typename T, typename W = int>
class Graph {
public:
    // Adjacency list: node -> list of outgoing edges
    using AdjList = std::unordered_map<T, std::vector<Edge<T, W>>>;

    // ---- Constructor ----
    
    // Creates a directed graph by default.
    // Pass false to create an undirected graph.
    explicit Graph(bool directed = true) : directed_(directed) {}

    // ---- Modifiers ----

    // Adds a node if it does not already exist.
    void addNode(const T& node) {
        adjacency_.emplace(node, std::vector<Edge<T, W>>{});
    }

    // Adds an edge from 'from' to 'to'.
    // For undirected graphs, also inserts the reverse edge.
    // Creates the destination node if it does not already exist (for directed graphs).
    void addEdge(const T& from, const T& to, W w = W{1}) {
        adjacency_[from].emplace_back(to, w);

        if (!directed_)
            adjacency_[to].emplace_back(from, w);

        else 
            adjacency_.emplace(to, std::vector<Edge<T, W>>{});
    }

    // Removes the edge from 'from' to 'to'.
    // For undirected graphs, removes the reverse edge as well.
    void removeEdge(const T& from, const T& to) {
        auto it = adjacency_.find(from);
        if (it == adjacency_.end()) return;

        auto& edges = it->second;
        edges.erase(
            std::remove_if(edges.begin(), edges.end(), 
                                            [&to](const Edge<T, W>& e) {return e.to == to; }),
            edges.end()
        );

        if(!directed_) {
            auto& rev = adjacency_[to];
            rev.erase(
                std::remove_if(rev.begin(), rev.end(),
                                            [&from](const Edge<T, W>& e) {return e.to == from; }),
                rev.end()
            );
        }
    }

    // ---- Queries ----
    
    // Returns all outgoing edges from a node.
    // Throws std::out_of_range if the node does not exist.
    const std::vector<Edge<T, W>>& neighbours(const T& node) const {
        auto it = adjacency_.find(node);

        if (it == adjacency_.end())
            throw std::out_of_range("Node not in graph");

        return it->second;
    }

    bool hasNode(const T& node) const {
        return adjacency_.count(node) > 0;
    }

    bool hasEdge(const T& from, const T& to) const {
        auto it = adjacency_.find(from);
        if (it == adjacency_.end()) return false;

        for (const auto& e : it->second)
            if (e.to == to) return true;

        return false;
    }

    // Returns a list of all nodes currently in the graph.
    std::vector<T> nodes() const {
        std::vector<T> result;
        result.reserve(adjacency_.size());

        for(const auto& [node, _] : adjacency_) {
            result.push_back(node);
        }

        return result;
    }

    int nodeCount() const { return static_cast<int>(adjacency_.size()); }
    bool isDirected() const { return directed_; }

    // Counts stored edges.
    // Undirected graphs store each edge twice,
    // so divide the total by two.
    int edgeCount() const {
        int total = 0;

        for(const auto& [_, edges] : adjacency_){
            total += static_cast<int>(edges.size());
        }

        return directed_ ? total : total / 2;
    }

    // Read-only access to the underlying adjacency list.
    const AdjList& adjacency() const { return adjacency_; }

    // Pretty-print the graph in adjacency-list form.
    friend std::ostream& operator<<(std::ostream& os, const Graph<T, W>& g) {
        os << (g.directed_ ? "Directed" : "Undirected")
           << " graph |V| = " << g.nodeCount()
           << "  |E| = " << g.edgeCount() << "\n";

        for (const auto& [node, edges] : g.adjacency_) {
            os << "  " << node << " -> ";

            for (const auto& e : edges)
                os << e.to << "(" << e.weight << ") ";

            os << "\n";
        }

        return os;
    }

private:
    AdjList adjacency_;   // node -> outgoing edges
    bool directed_;       // true = directed, false = undirected
};

} // namespace graphlib