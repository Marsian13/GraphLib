#pragma once
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <ostream>
#include <algorithm>

namespace graphlib {

template <typename T, typename W = int>
struct Edge {
    T to;       // destination node
    W weight;   // edge weight (default 1 for unweighted graphs)

    Edge(T destination, W w = W{1}) : to(destination), weight(w) {}
};

template <typename T, typename W = int>
class Graph {
public:
    using AdjList = std::unordered_map<T, std::vector<Edge<T, W>>>;

    explicit Graph(bool directed = true) : directed_(directed) {}

    void addNode(const T& node) {
        adjacency_.emplace(node, std::vector<Edge<T, W>>{});
    }

    void addEdge(const T& from, const T& to, W w = W{1}) {
        adjacency_[from].emplace_back(to, w);

        if (!directed_)
            adjacency_[to].emplace_back(from, w);

        else 
            adjacency_.emplace(to, std::vector<Edge<T, W>>{});
    }

    void removeEdge(const T& from, const T& to){
        auto it = adjacency_.find(from);
        if (it == adjacency_.end()) return;

        auto& edges = it->second;
        edges.erase(
            std::remove_if(edges.begin(), edges.end(), 
                                            [&to](const Edge<T, W>& e) {return e.to == to; }),
            edges.end()
        );

        if(!directed_){
            auto& rev = adjacency_[to];
            rev.erase(
                std::remove_if(rev.begin(), rev.end(),
                                            [&from](const Edge<T, W>& e) {return e.to == from; }),
                rev.end()
            );
        }
    }

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

    std::vector<T> nodes() const {
        std::vector<T> result;

        for(const auto& [node, _] : adjacency_) {
            result.push_back(node);
        }

        return result;
    }

    int nodeCount() const { return static_cast<int>(adjacency_.size()); }
    bool isDirected() const { return directed_; }

    int edgeCount() const {
        int total = 0;

        for(const auto& [_, edges] : adjacency_){
            total += static_cast<int>(edges.size());
        }

        return directed_ ? total : total / 2;
    }

    const AdjList& adjacency() const { return adjacency_; }

    friend std::ostream& operator<<(std::ostream& os, const Graph<T, W> g) {
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
    AdjList adjacency_;  // node -> list of outgoing edges
    bool    directed_;   // true for directed graphs
};

} // namespace graphlib