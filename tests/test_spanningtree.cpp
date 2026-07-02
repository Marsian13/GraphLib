#include <gtest/gtest.h>
#include "Graph.h"
#include "SpanningTree.h"
#include <string>
#include <vector>

using namespace graphlib;

// =========================================================
//  SPANNING TREE TESTS (Kruskal & Prim)
// =========================================================

TEST(SpanningTreeTest, KruskalAndPrimMatchExpectedWeight) {
    // MST algorithms require an UNDIRECTED graph
    Graph<std::string, int> g(false); 
    
    // Create a 4-node graph with a cycle
    //   A --1-- B
    //   | \     |
    //   4   5   2
    //   |     \ |
    //   D --3-- C
    g.addEdge("A", "B", 1);
    g.addEdge("B", "C", 2);
    g.addEdge("C", "D", 3);
    g.addEdge("D", "A", 4);
    g.addEdge("A", "C", 5);

    // The Minimum Spanning Tree should greedily pick:
    // A-B (1), B-C (2), C-D (3). 
    // Total weight = 1 + 2 + 3 = 6.

    auto kResult = kruskal(g);
    auto pResult = prim(g, std::string("A"));

    // 1. Both algorithms must find the exact same total minimum weight
    EXPECT_EQ(kResult.totalWeight, 6);
    EXPECT_EQ(pResult.totalWeight, 6);

    // 2. Both algorithms must use exactly V - 1 edges (4 nodes - 1 = 3 edges)
    EXPECT_EQ(kResult.edges.size(), 3);
    EXPECT_EQ(pResult.edges.size(), 3);
}

TEST(SpanningTreeTest, ThrowsOnDisconnectedGraph) {
    Graph<std::string, int> g(false);
    
    // Island 1
    g.addEdge("A", "B", 1);
    g.addEdge("B", "C", 2);
    
    // Island 2 (completely disconnected)
    g.addEdge("X", "Y", 3);

    // Because the graph is physically split into two islands, 
    // it is mathematically impossible to build a single Spanning Tree.
    // Our custom std::runtime_error guards should aggressively throw.
    EXPECT_THROW(kruskal(g), std::runtime_error);
    EXPECT_THROW(prim(g, std::string("A")), std::runtime_error);
}

TEST(SpanningTreeTest, HandlesStarGraphCorrectly) {
    Graph<int, int> g(false);

    // Star topology: Node 0 is the center, connected to 1, 2, 3, 4
    g.addEdge(0, 1, 10);
    g.addEdge(0, 2, 20);
    g.addEdge(0, 3, 30);
    g.addEdge(0, 4, 40);

    // Add some expensive outer edges that should be ignored by the MST
    g.addEdge(1, 2, 100);
    g.addEdge(3, 4, 100);

    auto kResult = kruskal(g);
    
    // The MST should perfectly trace the star from the center
    // Total: 10 + 20 + 30 + 40 = 100
    EXPECT_EQ(kResult.totalWeight, 100);
    
    // 5 nodes means exactly 4 edges in the MST
    EXPECT_EQ(kResult.edges.size(), 4);
}