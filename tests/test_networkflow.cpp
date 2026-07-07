#include <gtest/gtest.h>
#include "../include/Graph.h"
#include "../include/NetworkFlow.h"
#include <string>

using namespace graphlib;

// =========================================================
//  NETWORK FLOW TESTS (Edmonds-Karp)
// =========================================================

TEST(NetworkFlowTest, BasicBottleneck) {
    Graph<std::string, int> g(true); // Flow networks MUST be directed

    // S -> A (Capacity 10)
    // A -> T (Capacity 5)
    g.addEdge("S", "A", 10);
    g.addEdge("A", "T", 5);

    // The pipe from A to T is the bottleneck. 
    // Even though S can send 10, only 5 can make it to T.
    int maxFlow = edmondsKarp(g, std::string("S"), std::string("T"));
    EXPECT_EQ(maxFlow, 5);
}

TEST(NetworkFlowTest, CLRSTextbookExample) {
    Graph<std::string, int> g(true);
    
    // This is the exact complex flow network used in the 
    // standard "Introduction to Algorithms" (CLRS) textbook.
    // It requires the algorithm to use reverse/phantom edges 
    // to correct its own greedy mistakes and find the true max flow.
    
    g.addEdge("S", "V1", 16);
    g.addEdge("S", "V2", 13);
    
    g.addEdge("V1", "V3", 12);
    g.addEdge("V1", "V2", 10);
    
    g.addEdge("V2", "V1", 4);
    g.addEdge("V2", "V4", 14);
    
    g.addEdge("V3", "V2", 9);
    g.addEdge("V3", "T", 20);
    
    g.addEdge("V4", "V3", 7);
    g.addEdge("V4", "T", 4);

    // The mathematically proven maximum flow for this network is 23.
    int maxFlow = edmondsKarp(g, std::string("S"), std::string("T"));
    EXPECT_EQ(maxFlow, 23);
}

TEST(NetworkFlowTest, DisconnectedSinkYieldsZeroFlow) {
    Graph<std::string, int> g(true);
    
    g.addEdge("S", "A", 100);
    g.addEdge("A", "B", 50);
    
    g.addNode("T"); // Sink is completely isolated

    // If water cannot physically reach the sink, max flow must be 0.
    int maxFlow = edmondsKarp(g, std::string("S"), std::string("T"));
    EXPECT_EQ(maxFlow, 0);
}

TEST(NetworkFlowTest, MultiplePathsCombineFlow) {
    Graph<std::string, int> g(true);

    // Top path can handle 10
    g.addEdge("S", "Top", 10);
    g.addEdge("Top", "T", 10);

    // Bottom path can handle 15
    g.addEdge("S", "Bottom", 15);
    g.addEdge("Bottom", "T", 15);

    // Total flow should be perfectly additive (10 + 15 = 25)
    int maxFlow = edmondsKarp(g, std::string("S"), std::string("T"));
    EXPECT_EQ(maxFlow, 25);
}