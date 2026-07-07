# GraphLib

A header-only, templated C++17 graph algorithms library implementing traversal, shortest-path, spanning-tree, connectivity, and max-flow algorithms — all unit-tested with GoogleTest and built with CMake.

## Why this project

Most graph algorithm implementations online are single-purpose, hard-coded scripts. GraphLib is structured like a real library: a reusable `Graph<T, W>` template, clean separation of algorithms by responsibility, full unit test coverage, and complexity documented inline for every function.

## Features

| Module | Algorithms |
|---|---|
| **Traversal** | BFS, DFS, Topological Sort, Cycle Detection, Connected Components |
| **Shortest Path** | Dijkstra, Bellman-Ford, Floyd-Warshall |
| **Spanning Tree** | Union-Find (DSU), Kruskal's MST, Prim's MST |
| **Connectivity** | Tarjan's SCC, Kosaraju's SCC, Bridges, Articulation Points |
| **Network Flow** | Ford-Fulkerson method (Edmonds-Karp / BFS-based) |

## Complexity Reference

| Algorithm | Time | Space | Notes |
|---|---|---|---|
| BFS / DFS | O(V + E) | O(V) | |
| Topological Sort | O(V + E) | O(V) | DAG only |
| Dijkstra | O((V+E) log V) | O(V) | Non-negative weights only |
| Bellman-Ford | O(V · E) | O(V) | Handles negative weights, detects negative cycles |
| Floyd-Warshall | O(V³) | O(V²) | All-pairs shortest paths |
| Kruskal | O(E log E) | O(V + E) | Uses Union-Find |
| Prim | O((V+E) log V) | O(V) | Min-heap based |
| Tarjan SCC | O(V + E) | O(V) | Single DFS pass |
| Kosaraju SCC | O(V + E) | O(V) | Two DFS passes |
| Bridges / Articulation Points | O(V + E) | O(V) | Undirected graphs |
| Edmonds-Karp | O(V · E²) | O(V + E) | Max flow, BFS-based augmenting paths |

## Project Structure

```
GraphLib/
├── include/
│   ├── Graph.h            # Core templated graph (adjacency list)
│   ├── Traversal.h        # BFS, DFS, topo sort, cycle detection
│   ├── ShortestPath.h     # Dijkstra, Bellman-Ford, Floyd-Warshall
│   ├── SpanningTree.h     # Union-Find, Kruskal, Prim
│   ├── Connectivity.h     # Tarjan SCC, Kosaraju SCC, bridges, AP
│   └── NetworkFlow.h      # Edmonds-Karp max flow
├── tests/                 # GoogleTest unit tests (one file per module)
├── examples/
│   └── demo.cpp           # End-to-end usage demo of every algorithm
├── CMakeLists.txt
└── README.md
```

## Building

### Requirements
- C++17 compiler (g++ 9+ or clang 10+)
- CMake 3.14+
- GoogleTest (`libgtest-dev`) — optional, only needed for running tests

### Build & run the demo

```bash
mkdir build && cd build
cmake ..
make
./graphlib_demo
```

### Run the unit tests

```bash
cd build
ctest --output-on-failure
```

All 47 unit tests across 5 suites pass, covering normal cases, edge cases (empty/single-node graphs, disconnected graphs), and known textbook examples (e.g. the classic max-flow network with expected flow = 23).

### Quick compile without CMake

```bash
g++ -std=c++17 -Iinclude examples/demo.cpp -o demo
./demo
```

---

## API Reference

Every function lives inside `namespace graphlib`. Add `using namespace graphlib;` in your `.cpp` file or prefix calls with `graphlib::`.

---

### Graph.h — Core Graph Class

```cpp
#include "Graph.h"
```

#### `Graph<T, W>`

| Template Param | Meaning | Example |
|---|---|---|
| `T` | Node type — any hashable type | `int`, `char`, `std::string` |
| `W` | Weight type — defaults to `int` | `int`, `double` |

**Constructor**
```cpp
Graph<T, W> g(bool directed);

Graph<int> g(true);          // directed, int nodes, int weights
Graph<std::string> g(false); // undirected, string nodes, int weights
Graph<char, double> g(true); // directed, char nodes, double weights
```

**Adding nodes and edges**
```cpp
void addNode(const T& node);
void addEdge(const T& from, const T& to, W weight = 1);

g.addNode(5);                  // add isolated node
g.addEdge(1, 2);               // weight defaults to 1
g.addEdge("Delhi", "Mumbai", 1200);  // weight = 1200
```

**Removing edges**
```cpp
void removeEdge(const T& from, const T& to);

g.removeEdge(1, 2);
```

**Querying**
```cpp
bool hasNode(const T& node);
bool hasEdge(const T& from, const T& to);
int  nodeCount();
int  edgeCount();
bool isDirected();

const std::vector<Edge<T,W>>& neighbours(const T& node); // throws if node not found
std::vector<T> nodes();                                   // all nodes as a vector
```

**Printing**
```cpp
std::cout << g;  // prints node count, edge count, and adjacency list
```

---

### Traversal.h

```cpp
#include "Traversal.h"
```

**BFS — visits nodes layer by layer**
```cpp
std::vector<T> bfs(const Graph<T,W>& graph, const T& start);

auto order = bfs(g, 1);   // returns nodes in BFS visit order
```

**DFS — goes deep before backtracking**
```cpp
std::vector<T> dfs(const Graph<T,W>& graph, const T& start);

auto order = dfs(g, 1);   // returns nodes in DFS visit order
```

**DFS with callback — run custom logic on each node**
```cpp
void dfsCallback(const Graph<T,W>& graph,
                 const T& start,
                 const std::function<void(const T&)>& onVisit);

dfsCallback(g, 1, [](const int& node) {
    std::cout << node << "\n";
});
```

**Cycle detection — directed graphs only**
```cpp
bool hasCycle(const Graph<T,W>& graph);

if (hasCycle(g)) { /* graph has a cycle */ }
```

**Topological sort — DAGs only, throws if cycle exists**
```cpp
std::vector<T> topologicalSort(const Graph<T,W>& graph);

auto order = topologicalSort(g);  // valid dependency order
```

**Connected components — undirected graphs**
```cpp
std::vector<std::vector<T>> connectedComponents(const Graph<T,W>& graph);

auto components = connectedComponents(g);
// components[0] = first group, components[1] = second group, ...
```

---

### ShortestPath.h

```cpp
#include "ShortestPath.h"
```

**Result type — returned by Dijkstra and Bellman-Ford**
```cpp
struct ShortestPathResult<T, W> {
    std::unordered_map<T, W> dist;  // dist[v] = shortest distance from source to v
    std::unordered_map<T, T> prev;  // prev[v] = previous node on shortest path

    std::vector<T> reconstructPath(const T& source, const T& target);
    // returns [] if target unreachable
};
```

**Dijkstra — non-negative weights only**
```cpp
ShortestPathResult<T,W> dijkstra(const Graph<T,W>& graph, const T& source);

auto result = dijkstra(g, 'A');
int dist     = result.dist['D'];                    // shortest distance A→D
auto path    = result.reconstructPath('A', 'D');    // e.g. ['A', 'C', 'D']
```

**Bellman-Ford — handles negative weights, detects negative cycles**
```cpp
std::pair<ShortestPathResult<T,W>, bool>
bellmanFord(const Graph<T,W>& graph, const T& source);
// second value = true if a negative cycle was detected

auto [result, hasNegCycle] = bellmanFord(g, 'A');
if (hasNegCycle) { /* distances are invalid */ }
```

**Floyd-Warshall — all-pairs shortest paths**
```cpp
FloydWarshallResult<T,W> floydWarshall(const Graph<T,W>& graph);

auto result = floydWarshall(g);
int dist    = result.getDistance('A', 'D');   // shortest A→D
bool hasCyc = result.hasNegativeCycle();
```

**Unreachable node sentinel**
```cpp
graphlib::INF<int>()     // maximum int / 2  (safe to add without overflow)
graphlib::INF<double>()  // maximum double / 2
```

---

### SpanningTree.h

```cpp
#include "SpanningTree.h"
```

**UnionFind — disjoint set union**
```cpp
UnionFind<T> uf(const std::vector<T>& nodes);

T    root      = uf.find(x);          // root representative of x's component
bool merged    = uf.unite(x, y);      // true if they were in different components
bool connected = uf.connected(x, y);  // true if same component
```

**MST result type**
```cpp
struct MSTResult<T, W> {
    std::vector<std::pair<T,T>> edges;  // edges in the MST as (from, to) pairs
    W totalWeight;                       // sum of all MST edge weights
};
```

**Kruskal — sort edges, use Union-Find to avoid cycles**
```cpp
MSTResult<T,W> kruskal(const Graph<T,W>& graph);
// throws if graph is not connected

auto mst = kruskal(g);
std::cout << mst.totalWeight;  // minimum total weight
```

**Prim — grow MST from a seed node**
```cpp
MSTResult<T,W> prim(const Graph<T,W>& graph, const T& start);
// throws if graph is not connected

auto mst = prim(g, 1);
for (auto& [u, v] : mst.edges) { /* each edge in MST */ }
```

---

### Connectivity.h

```cpp
#include "Connectivity.h"
```

**Tarjan's SCC — single DFS pass**
```cpp
std::vector<std::vector<T>> tarjanSCC(const Graph<T,W>& graph);

auto sccs = tarjanSCC(g);
// sccs[0] = first strongly connected component, etc.
```

**Kosaraju's SCC — two DFS passes (cross-check alternative)**
```cpp
std::vector<std::vector<T>> kosarajuSCC(const Graph<T,W>& graph);

auto sccs = kosarajuSCC(g);  // same result as tarjanSCC
```

**Transpose — reverse every edge (helper used by Kosaraju)**
```cpp
Graph<T,W> transpose(const Graph<T,W>& graph);
```

**Bridges — edges whose removal disconnects the graph**
```cpp
std::vector<std::pair<T,T>> findBridges(const Graph<T,W>& graph);
// undirected graphs only

auto bridges = findBridges(g);
// bridges[i] = {u, v} means edge u-v is a bridge
```

**Articulation points — nodes whose removal disconnects the graph**
```cpp
std::vector<T> findArticulationPoints(const Graph<T,W>& graph);
// undirected graphs only

auto aps = findArticulationPoints(g);
```

---

### NetworkFlow.h

```cpp
#include "NetworkFlow.h"
```

**Edmonds-Karp max flow**
```cpp
W edmondsKarp(const Graph<T,W>& graph, const T& source, const T& sink);

int flow = edmondsKarp(g, 'S', 'T');  // maximum flow from S to T
```

---

## Full Usage Example

```cpp
#include "Graph.h"
#include "Traversal.h"
#include "ShortestPath.h"
#include "SpanningTree.h"
#include "Connectivity.h"
#include "NetworkFlow.h"
#include <iostream>

using namespace graphlib;

int main() {
    // --- Shortest path on a weighted directed graph ---
    Graph<char, int> g(true);
    g.addEdge('A', 'B', 4);
    g.addEdge('A', 'C', 1);
    g.addEdge('C', 'B', 2);
    g.addEdge('B', 'D', 1);

    auto sp = dijkstra(g, 'A');
    std::cout << sp.dist['D'] << "\n";              // 4
    for (char n : sp.reconstructPath('A', 'D'))
        std::cout << n << " ";                      // A C B D

    // --- Topological sort on a DAG ---
    Graph<std::string> dag(true);
    dag.addEdge("Math", "Physics");
    dag.addEdge("Physics", "Capstone");
    for (auto& n : topologicalSort(dag))
        std::cout << n << " ";                      // Math Physics Capstone

    // --- MST on undirected graph ---
    Graph<int, int> ug(false);
    ug.addEdge(1, 2, 4);
    ug.addEdge(1, 3, 1);
    ug.addEdge(2, 3, 2);
    auto mst = kruskal(ug);
    std::cout << mst.totalWeight << "\n";           // 3

    // --- Max flow ---
    Graph<char, int> fg(true);
    fg.addEdge('S', 'A', 10);
    fg.addEdge('A', 'T', 10);
    fg.addEdge('S', 'B', 5);
    fg.addEdge('B', 'T', 5);
    std::cout << edmondsKarp(fg, 'S', 'T') << "\n"; // 15

    return 0;
}
```

---

## Design Decisions

- **Adjacency list over matrix**: most real-world graphs are sparse (E << V²); adjacency list gives O(V+E) space instead of O(V²).
- **Templated on node type `T`**: works with `int`, `char`, `std::string`, or any hashable custom type — not hard-coded to integer indices.
- **Header-only**: no separate compilation step; simplifies integration into any project.
- **Algorithms as free functions, not Graph methods**: keeps `Graph` focused on data storage (single responsibility principle) and makes the library easy to extend.
- **Negative-cycle detection** built into Bellman-Ford and Floyd-Warshall rather than silently returning wrong answers.