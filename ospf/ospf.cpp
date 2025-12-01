#include <bits/stdc++.h>
using namespace std;

// Using long long for distances (costs)
using ll = long long;

// A very large number used to initialize distances
const ll INF = (ll)1e15;

/*
    -------------------------------------------------------------------------
    FUNCTION: dijkstra()
    PURPOSE : Compute shortest path distances from a given source router
              to all other routers in the network using Dijkstra's algorithm.

    WHY DIJKSTRA?
    - OSPF uses link-state routing.
    - Each router runs SPF (Shortest Path First) algorithm.
    - Dijkstra is the classical SPF algorithm.

    PARAMETERS:
    - src:      source router (1 to n)
    - adj:      adjacency list where adj[u] contains pairs (v, weight)
    - parent:   vector storing the predecessor of each node in shortest path tree
                This is crucial for next-hop calculation.

    RETURNS:
    - dist: vector containing the minimum path cost from src to every node
    -------------------------------------------------------------------------
*/
vector<ll> dijkstra(int src, const vector<vector<pair<int,ll>>> &adj, vector<int> &parent) {
    int n = adj.size() - 1;

    // Step 1: Initialize distances to INF
    vector<ll> dist(n + 1, INF);

    // Step 2: Initialize parent array for shortest-path tree
    parent.assign(n + 1, -1);

    // Distance to self = 0
    dist[src] = 0;

    /*
        Min-priority queue (min-heap):
        Stores pairs (distance, node).
        Always extracts the node with the smallest known distance.
    */
    priority_queue<
        pair<ll,int>,
        vector<pair<ll,int>>,
        greater<pair<ll,int>>
    > pq;

    // Insert the source node in the PQ
    pq.push(make_pair(0, src));

    // Dijkstra's main loop
    while (!pq.empty()) {
        auto top = pq.top();
        pq.pop();

        ll d = top.first;   // known shortest distance
        int u = top.second; // node

        // If this entry is outdated, skip it
        if (d != dist[u]) continue;

        /*
            Relaxation Step:
            For every neighbor v of u, check if we can improve dist[v]
            using the path src → ... → u → v.
        */
        for (auto &edge : adj[u]) {
            int v = edge.first;
            ll w = edge.second;

            // If a shorter path is found, update it
            if (dist[v] > dist[u] + w) {
                dist[v] = dist[u] + w;
                parent[v] = u;              // store predecessor for SPT
                pq.push(make_pair(dist[v], v));  // push new distance to PQ
            }
        }
    }

    return dist;  // return the computed shortest distances
}

/*
    -------------------------------------------------------------------------
    FUNCTION: next_hop()
    PURPOSE : Determine the immediate next-hop router for forwarding packets
              from source 'src' to destination 'dest'.

    HOW DOES OSPF CHOOSE NEXT HOP?
    - It constructs a shortest-path tree using Dijkstra.
    - Using the parent[] array, we backtrack from destination to source.
    - The FIRST router after the source is the next hop.

    EXAMPLE:
    If path is 1 → 3 → 7 → 5
    For src = 1, dest = 5, next hop = 3.

    PARAMETERS:
    - src    : source router
    - dest   : destination router
    - parent : parent array filled by Dijkstra

    RETURNS:
    - Router number of next hop, or -1 if dest is unreachable
    -------------------------------------------------------------------------
*/
int next_hop(int src, int dest, const vector<int> &parent) {
    // Next hop to itself is itself
    if (src == dest) return src;

    // If no path exists
    if (parent[dest] == -1) return -1;

    int cur = dest;
    int prev = parent[cur];

    // Move upward until reaching src
    while (prev != src && prev != -1) {
        cur = prev;
        prev = parent[cur];
    }

    if (prev == -1) return -1; // unreachable

    return cur; // cur is the first router after src
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m;

    /*
        n = number of routers (nodes)
        m = number of bidirectional links (edges)
    */
    cin >> n >> m;

    /*
        Adjacency list representation of graph.
        adj[u] contains (v, w) meaning link u ↔ v with cost w.
    */
    vector<vector<pair<int,ll>>> adj(n + 1);

    // Read all edges
    for (int i = 0; i < m; i++) {
        int u, v;
        ll w;

        cin >> u >> v >> w;

        // Since OSPF uses undirected graph, we insert both ways
        adj[u].push_back(make_pair(v, w));
        adj[v].push_back(make_pair(u, w));
    }

    cout << "OSPF-style routing tables (Destination, Cost, NextHop)\n\n";

    /*
        For EACH router in the network,
        run Dijkstra and generate a routing table.

        This simulates EXACTLY how OSPF works:
        - Each router independently calculates SPF tree.
        - Each router generates its own routing table.
    */
    for (int src = 1; src <= n; ++src) {

        vector<int> parent;

        // Run Dijkstra from this router
        auto dist = dijkstra(src, adj, parent);

        cout << "Routing table for node " << src << ":\n";
        cout << "Dest\tCost\tNextHop\n";

        // For every possible destination router
        for (int dest = 1; dest <= n; ++dest) {

            // If unreachable
            if (dist[dest] == INF) {
                cout << dest << "\t-\t-\n";
                continue;
            }

            // Compute next hop using parent[] chain
            int nh = next_hop(src, dest, parent);

            // Print entry
            if (nh == -1)
                cout << dest << "\t" << dist[dest] << "\t-\n";
            else
                cout << dest << "\t" << dist[dest] << "\t" << nh << "\n";
        }

        cout << "\n";  // separate tables
    }

    return 0;
}


// 4 5
// 1 2 2
// 1 3 5
// 2 3 1
// 2 4 3
// 3 4 2
// OSPF-style routing tables (Destination, Cost, NextHop)

// Routing table for node 1:
// Dest    Cost    NextHop
// 1       0       1
// 2       2       2
// 3       3       2
// 4       5       2

// Routing table for node 2:
// Dest    Cost    NextHop
// 1       2       1
// 2       0       2
// 3       1       3
// 4       3       4

// Routing table for node 3:
// Dest    Cost    NextHop
// 1       3       2
// 2       1       2
// 3       0       3
// 4       2       4

// Routing table for node 4:
// Dest    Cost    NextHop
// 1       5       2
// 2       3       2
// 3       2       3
// 4       0       4