#include <gtest/gtest.h>
#include "Graph.h"
#include "ShortestPath.h"

using namespace graphlib;

// ---------------- Dijkstra tests ----------------

TEST(Dijkstra, ComputesCorrectDistances) {
    Graph<char, int> g(true);
    g.addEdge('A', 'B', 4);
    g.addEdge('A', 'C', 1);
    g.addEdge('C', 'B', 2);
    g.addEdge('B', 'D', 1);

    auto result = dijkstra(g, 'A');
    EXPECT_EQ(result.dist['A'], 0);
    EXPECT_EQ(result.dist['C'], 1);
    EXPECT_EQ(result.dist['B'], 3); // via C, not direct (4)
    EXPECT_EQ(result.dist['D'], 4);
}

TEST(Dijkstra, UnreachableNodeStaysInfinite) {
    Graph<char, int> g(true);
    g.addEdge('A', 'B', 1);
    g.addNode('Z'); // isolated

    auto result = dijkstra(g, 'A');
    EXPECT_EQ(result.dist['Z'], INF<int>());
}

TEST(Dijkstra, ReconstructPathIsCorrect) {
    Graph<char, int> g(true);
    g.addEdge('A', 'B', 4);
    g.addEdge('A', 'C', 1);
    g.addEdge('C', 'B', 2);

    auto result = dijkstra(g, 'A');
    auto path = result.reconstructPath('A', 'B');
    std::vector<char> expected = {'A', 'C', 'B'};
    EXPECT_EQ(path, expected);
}

TEST(Dijkstra, SingleNodeGraph) {
    Graph<char, int> g(true);
    g.addNode('A');
    auto result = dijkstra(g, 'A');
    EXPECT_EQ(result.dist['A'], 0);
}

// ---------------- Bellman-Ford tests ----------------

TEST(BellmanFord, HandlesNegativeWeightsCorrectly) {
    Graph<char, int> g(true);
    g.addEdge('A', 'B', 5);
    g.addEdge('A', 'C', 2);
    g.addEdge('C', 'B', -3); // negative edge, but no negative cycle

    auto [result, negCycle] = bellmanFord(g, 'A');
    EXPECT_FALSE(negCycle);
    EXPECT_EQ(result.dist['B'], -1); // 2 + (-3) beats direct 5
}

TEST(BellmanFord, DetectsNegativeCycle) {
    Graph<char, int> g(true);
    g.addEdge('A', 'B', 1);
    g.addEdge('B', 'C', -1);
    g.addEdge('C', 'A', -1); // total cycle weight = -1 < 0

    auto [result, negCycle] = bellmanFord(g, 'A');
    EXPECT_TRUE(negCycle);
}

TEST(BellmanFord, MatchesDijkstraOnNonNegativeGraph) {
    Graph<char, int> g(true);
    g.addEdge('A', 'B', 4);
    g.addEdge('A', 'C', 1);
    g.addEdge('C', 'B', 2);

    auto dij = dijkstra(g, 'A');
    auto [bf, negCycle] = bellmanFord(g, 'A');

    EXPECT_FALSE(negCycle);
    EXPECT_EQ(dij.dist['B'], bf.dist['B']);
}

// ---------------- Floyd-Warshall tests ----------------

TEST(FloydWarshall, AllPairsCorrect) {
    Graph<char, int> g(true);
    g.addEdge('A', 'B', 3);
    g.addEdge('B', 'C', 2);
    g.addEdge('A', 'C', 10);

    auto result = floydWarshall(g);
    EXPECT_EQ(result.getDistance('A', 'C'), 5); // via B: 3+2 < 10
    EXPECT_EQ(result.getDistance('A', 'A'), 0);
}

TEST(FloydWarshall, DetectsNegativeCycle) {
    Graph<char, int> g(true);
    g.addEdge('A', 'B', 1);
    g.addEdge('B', 'A', -3); // 2-cycle with negative total

    auto result = floydWarshall(g);
    EXPECT_TRUE(result.hasNegativeCycle());
}