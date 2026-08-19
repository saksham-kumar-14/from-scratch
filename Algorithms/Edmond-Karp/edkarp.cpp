#include <bits/stdc++.h>
using namespace std;

struct Edge {
    int v;            // destination node
    int flow;         // current flow
    int capacity;     // max capacity
    int rev;          // index of reverse edge in adj[v]
};

class Graph {
    int V;
    vector<vector<Edge>> adj;

public:
    Graph(int n) {
        V = n;
        adj.assign(n, {});
    }

    void add(int u, int v, int cap) {
        // Forward edge u -> v with capacity = cap
        Edge forward_edge = {v, 0, cap, (int)adj[v].size()};
        // Residual edge v -> u with capacity = 0
        Edge back_edge = {u, 0, 0, (int)adj[u].size()};

        adj[u].push_back(forward_edge);
        adj[v].push_back(back_edge);
    }

    // BFS to find an augmenting path with available capacity
    int bfs(int s, int t, vector<pair<int, int>>& parent) {
        fill(parent.begin(), parent.end(), make_pair(-1, -1));
        parent[s] = {s, -1};

        queue<pair<int, int>> q;
        q.push({s, INT_MAX}); // {current_node, bottleneck_capacity}

        while (!q.empty()) {
            auto [u, flow] = q.front();
            q.pop();

            for (int i = 0; i < (int)adj[u].size(); ++i) {
                Edge& edge = adj[u][i];
                // Check if target node is unvisited and residual capacity > 0
                if (parent[edge.v].first == -1 && edge.capacity - edge.flow > 0) {
                    parent[edge.v] = {u, i};
                    int new_flow = min(flow, edge.capacity - edge.flow);

                    if (edge.v == t) return new_flow; // Reached sink

                    q.push({edge.v, new_flow});
                }
            }
        }
        return 0; // No augmenting path found
    }

    int max_flow(int s, int t) {
        int flow = 0;
        vector<pair<int, int>> parent(V); // Stores {parent_node, edge_index}

        int currentFlow = 0;
        while ((currentFlow = bfs(s, t, parent)) > 0) {
            flow += currentFlow;
            int u = t;

            // Backtrack from sink to source and update residual capacities
            while (u != s) {
                auto [p, idx] = parent[u];
                adj[p][idx].flow += currentFlow;

                int rev_idx = adj[p][idx].rev;
                adj[u][rev_idx].flow -= currentFlow;

                u = p;
            }
        }
        return flow;
    }
};

int main() {
    int n, m;
    cin >> n >> m;

    Graph g(n);
    for (int i = 0; i < m; ++i) {
        int u, v, cap;
        cin >> u >> v >> cap;
        g.add(u, v, cap);
    }

    int s, t;
    cin >> s >> t;
    cout << g.max_flow(s, t) << '\n';

    return 0;
}
