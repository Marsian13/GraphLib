#include <gtest/gtest.h>
#include "Graph.h"
#include "Connectivity.h"
#include <algorithm>
#include <vector>

using namespace graphlib;

// =========================================================
//  STRONGLY CONNECTED COMPONENTS (SCC) TESTS
// =========================================================

TEST(ConnectivityTest, TarjanAndKosarajuMatch) {
    Graph<int, int> g(true); // Directed graph

    // Create a graph with two separate SCCs connected by a one-way street
    // SCC 1: Nodes 0, 1, 2 (A triangle)
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 0);

    // One-way street from SCC 1 to SCC 2
    g.addEdge(2, 3);

    // SCC 2: Nodes 3, 4 (A pair bouncing back and forth)
    g.addEdge(3, 4);
    g.addEdge(4, 3);

    auto tarjanResult = tarjanSCC(g);
    auto kosarajuResult = kosarajuSCC(g);

    // Because Tarjan and Kosaraju explore the graph in different orders,
    // the arrays might be out of order (e.g., [[3,4], [0,1,2]] vs [[0,1,2], [3,4]]).
    // We write a quick lambda to sort the inner components and outer list so we can strictly compare them.
    auto normalize = [](std::vector<std::vector<int>>& sccs) {
        for (auto& comp : sccs) {
            std::sort(comp.begin(), comp.end());
        }
        std::sort(sccs.begin(), sccs.end());
    };

    normalize(tarjanResult);
    normalize(kosarajuResult);

    // 1. Both algorithms must find exactly 2 Strongly Connected Components
    EXPECT_EQ(tarjanResult.size(), 2);
    EXPECT_EQ(kosarajuResult.size(), 2);

    // 2. Both algorithms must output the exact same node groupings
    EXPECT_EQ(tarjanResult, kosarajuResult);
    
    // 3. Verify the exact contents of the groupings
    std::vector<std::vector<int>> expected = {{0, 1, 2}, {3, 4}};
    EXPECT_EQ(tarjanResult, expected);
}

// =========================================================
//  BRIDGE DETECTION TESTS
// =========================================================

TEST(ConnectivityTest, FindsBridgesInUndirectedGraph) {
    Graph<int, int> g(false); // Undirected graph

    // Triangle 1
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 0);

    // The Bridge! (Deleting this disconnects the graph)
    g.addEdge(2, 3);

    // Triangle 2
    g.addEdge(3, 4);
    g.addEdge(4, 5);
    g.addEdge(5, 3);

    auto bridges = findBridges(g);

    // There should be exactly 1 bridge
    ASSERT_EQ(bridges.size(), 1);

    // The bridge should be the edge between 2 and 3. 
    // Since it's undirected, it could be reported as (2,3) or (3,2)
    bool isCorrectBridge = (bridges[0].first == 2 && bridges[0].second == 3) || 
                           (bridges[0].first == 3 && bridges[0].second == 2);
                           
    EXPECT_TRUE(isCorrectBridge);
}

// =========================================================
//  ARTICULATION POINT TESTS
// =========================================================

TEST(ConnectivityTest, FindsArticulationPoints) {
    Graph<int, int> g(false); // Undirected graph

    // Same double-triangle graph as the Bridge test
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 0);
    
    g.addEdge(2, 3); // Bridge edge
    
    g.addEdge(3, 4);
    g.addEdge(4, 5);
    g.addEdge(5, 3);

    auto points = findArticulationPoints(g);

    // Nodes 2 and 3 are the gateways holding the two triangles together. 
    // If either node blows up, the graph shatters.
    ASSERT_EQ(points.size(), 2);

    // Sort to make the comparison easy
    std::sort(points.begin(), points.end());
    std::vector<int> expected = {2, 3};
    
    EXPECT_EQ(points, expected);
}

TEST(ConnectivityTest, ArticulationPointRootEdgeCase) {
    Graph<int, int> g(false); // Undirected graph
    
    // A simple line: 0 -- 1 -- 2
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    
    auto points = findArticulationPoints(g);
    
    // Node 1 is the only articulation point. 
    // This perfectly tests that the DFS "root" node (Node 0) is properly ignored 
    // since it only has 1 DFS child.
    ASSERT_EQ(points.size(), 1);
    EXPECT_EQ(points[0], 1);
}