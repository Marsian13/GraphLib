#include <gtest/gtest.h>
#include "Graph.h"

using namespace graphlib;

// A simple test case block
TEST(GraphTest, AddsDirectedEdgeCorrectly) {
    Graph<std::string, int> g(true);

    g.addEdge("Delhi", "Mumbai", 1200);
    g.addEdge("Delhi", "HYD", 1270);
    std::cout << g; 
    
    EXPECT_EQ(g.nodeCount(), 3);
    EXPECT_TRUE(g.hasEdge("Delhi", "Mumbai"));
    EXPECT_FALSE(g.hasEdge("Mumbai", "Delhi")); 
}

// Another test block testing the sink node logic
TEST(GraphTest, HandlesSinkNodesSafely) {
    Graph<std::string, int> g(true);
    g.addEdge("A", "B");
    
    EXPECT_NO_THROW(g.neighbours("B")); 
}