#include <gtest/gtest.h>
#include "Graph.h"
#include "ShortestPath.h"
#include <string>
#include <vector>

using namespace graphlib;

// =========================================================
//  DIJKSTRA TESTS
// =========================================================

TEST(ShortestPathTest, DijkstraBasicRouting) {
    Graph<std::string, int> g(true); // Directed graph
    
    // Setup a classic routing graph
    g.addEdge("A", "B", 4);
    g.addEdge("A", "C", 1);
    g.addEdge("C", "B", 2); // A -> C -> B is cost 3 (better than A -> B cost 4)
    g.addEdge("B", "D", 1);
    g.addEdge("C", "D", 5);

    auto result = dijkstra(g, std::string("A"));

    // 1. Check Distances
    EXPECT_EQ(result.dist.at("A"), 0);
    EXPECT_EQ(result.dist.at("C"), 1);
    EXPECT_EQ(result.dist.at("B"), 3);
    EXPECT_EQ(result.dist.at("D"), 4);

    // 2. Check Path Reconstruction
    std::vector<std::string> expectedPath = {"A", "C", "B", "D"};
    EXPECT_EQ(result.reconstructPath("A", "D"), expectedPath);
}

TEST(ShortestPathTest, DijkstraUnreachableNode) {
    Graph<std::string, int> g(true);
    g.addEdge("A", "B", 5);
    g.addNode("Isolated"); // No edges connect to this

    auto result = dijkstra(g, std::string("A"));

    // Distance should be infinity
    EXPECT_EQ(result.dist.at("Isolated"), INF<int>());
    
    // Path should be empty
    EXPECT_TRUE(result.reconstructPath("A", "Isolated").empty());
}

// =========================================================
//  BELLMAN-FORD TESTS
// =========================================================

TEST(ShortestPathTest, BellmanFordNegativeWeights) {
    Graph<std::string, int> g(true);
    
    // Negative weight, but NO negative cycle
    g.addEdge("A", "B", 4);
    g.addEdge("A", "C", 5);
    g.addEdge("C", "B", -2); // A -> C -> B is cost 3

    auto [result, hasCycle] = bellmanFord(g, std::string("A"));

    EXPECT_FALSE(hasCycle);
    EXPECT_EQ(result.dist.at("B"), 3);
}

TEST(ShortestPathTest, BellmanFordDetectsNegativeCycle) {
    Graph<std::string, int> g(true);
    
    // Create a negative cycle: A -> B -> C -> A (Total weight: -1)
    g.addEdge("A", "B", 1);
    g.addEdge("B", "C", -1);
    g.addEdge("C", "A", -1);

    auto [result, hasCycle] = bellmanFord(g, std::string("A"));

    // The algorithm must flag this as an invalid graph for shortest paths
    EXPECT_TRUE(hasCycle);
}

// =========================================================
//  FLOYD-WARSHALL TESTS
// =========================================================

TEST(ShortestPathTest, FloydWarshallAllPairs) {
    Graph<std::string, int> g(true);
    
    g.addEdge("1", "2", 3);
    g.addEdge("2", "3", 2);
    g.addEdge("1", "3", 6); // Direct is 6, but 1->2->3 is 5

    auto result = floydWarshall(g);

    EXPECT_FALSE(result.hasNegativeCycle());
    
    // Distance from itself is 0
    EXPECT_EQ(result.getDistance("1", "1"), 0);
    
    // Better path found through node 2
    EXPECT_EQ(result.getDistance("1", "3"), 5);
    
    // 3 to 1 is unreachable in this directed graph
    EXPECT_EQ(result.getDistance("3", "1"), INF<int>());
}