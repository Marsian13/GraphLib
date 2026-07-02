#include <gtest/gtest.h>
#include "Graph.h"
#include "Traversal.h"
#include <string>
#include <vector>
#include <algorithm>

using namespace graphlib;

// =========================================================
//  BFS & DFS TESTS
// =========================================================

TEST(TraversalTest, BFSOffersLevelOrder) {
    Graph<std::string, int> g(true);
    
    // Graph structure: 
    //      A
    //     / \.
    //    B   C
    //   /     \.
    //  D       E
    g.addEdge("A", "B");
    g.addEdge("A", "C");
    g.addEdge("B", "D");
    g.addEdge("C", "E");

    std::vector<std::string> result = bfs(g, std::string("A"));

    // Expected BFS order: Level 0 (A), Level 1 (B, C), Level 2 (D, E)
    std::vector<std::string> expected = {"A", "B", "C", "D", "E"};
    EXPECT_EQ(result, expected);
}

TEST(TraversalTest, DFSOffersDeepOrder) {
    Graph<std::string, int> g(true);
    
    // Same graph structure as BFS
    g.addEdge("A", "B");
    g.addEdge("A", "C");
    g.addEdge("B", "D");
    g.addEdge("C", "E");

    std::vector<std::string> result = dfs(g, std::string("A"));

    // Expected DFS order: Plunge down B's path first, then C's path
    std::vector<std::string> expected = {"A", "B", "D", "C", "E"};
    EXPECT_EQ(result, expected);
}

TEST(TraversalTest, DFSCallbackExecutesProperly) {
    Graph<int, int> g(true);
    g.addEdge(1, 2);
    g.addEdge(1, 3);
    g.addEdge(2, 4);

    int sum = 0;
    
    // The lambda function we pass into the traversal
    std::function<void(const int&)> addUp = [&](const int& node) {
        sum += node;
    };

    dfsCallback(g, 1, addUp);

    // Nodes visited: 1, 2, 3, 4. Sum should be 10.
    EXPECT_EQ(sum, 10);
}

// =========================================================
//  CYCLE DETECTION TESTS
// =========================================================

TEST(TraversalTest, DetectsNoCycleInDAG) {
    Graph<std::string, int> g(true);
    // Directed Acyclic Graph (DAG)
    g.addEdge("A", "B");
    g.addEdge("B", "C");
    g.addEdge("A", "C"); // Valid shortcut, not a cycle!

    EXPECT_FALSE(hasCycle(g));
}

TEST(TraversalTest, DetectsCycleInDirectedGraph) {
    Graph<std::string, int> g(true);
    
    // Cycle: A -> B -> C -> A
    g.addEdge("A", "B");
    g.addEdge("B", "C");
    g.addEdge("C", "A");

    EXPECT_TRUE(hasCycle(g));
}

// =========================================================
//  TOPOLOGICAL SORT TESTS
// =========================================================

TEST(TraversalTest, TopologicalSortValidOrder) {
    Graph<std::string, int> g(true);
    
    // Shoes and Socks analogy
    g.addEdge("Socks", "Shoes");
    g.addEdge("Underwear", "Pants");
    g.addEdge("Pants", "Shoes");

    std::vector<std::string> result = topologicalSort(g);

    // Find the positions of the items in the sorted result
    auto pos = [&](const std::string& item) {
        return std::distance(result.begin(), std::find(result.begin(), result.end(), item));
    };

    // Shoes must come strictly after both Socks and Pants
    EXPECT_TRUE(pos("Socks") < pos("Shoes"));
    EXPECT_TRUE(pos("Pants") < pos("Shoes"));
    // Pants must come strictly after Underwear
    EXPECT_TRUE(pos("Underwear") < pos("Pants"));
}

TEST(TraversalTest, TopologicalSortThrowsOnCycle) {
    Graph<int, int> g(true);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(3, 1); // Cycle!

    // Expect the algorithm to throw a std::runtime_error
    EXPECT_THROW(topologicalSort(g), std::runtime_error);
}

// =========================================================
//  CONNECTED COMPONENTS TESTS
// =========================================================

TEST(TraversalTest, FindsSeparateIslandsInUndirectedGraph) {
    Graph<std::string, int> g(false); // MUST be undirected
    
    // Island 1: USA
    g.addEdge("New York", "Chicago");
    g.addEdge("Chicago", "Denver");
    
    // Island 2: Europe
    g.addEdge("London", "Paris");
    
    // Island 3: Isolated node
    g.addNode("Tokyo");

    auto components = connectedComponents(g);

    // There should be exactly 3 disconnected components (islands)
    EXPECT_EQ(components.size(), 3);
}