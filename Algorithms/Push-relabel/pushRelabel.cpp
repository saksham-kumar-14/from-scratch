#include <bits/stdc++.h>
using namespace std;

struct Edge {
    int to;          // destination node
    int flow;        // current flow through edge
    int capacity;    // max capacity
    int rev;         // index of the reverse edge in adj[to]
};

class Graph {
    int n;
    vector<vector<Edge>> adj;
    vector<int> height;
    vector<int> excess;
    vector<int> seen;
    queue<int> excess_nodes;

public:
    Graph(int nodes){
        n = nodes;
        adj.assign(n, {});
        height.assign(n, 0);
        excess.assign(n, 0);
        seen.assign(n, 0);
    }

    void add(int u, int v, int capacity){
        Edge a = {v, 0, capacity, (int)adj[v].size()};
        Edge b = {u, 0, 0, (int)adj[u].size()};
        adj[u].push_back(a);
        adj[v].push_back(b);
    }

    void push(int u, Edge &e){
        int d = min(excess[u], e.capacity - e.flow);
        e.flow += d;
        adj[e.to][e.rev].flow -= d;
        excess[u] -= d;
        excess[e.to] += d;
    }

    void relabel(int u){
        int min_height = INT_MAX;
        for (auto &e : adj[u]) {
            if (e.capacity - e.flow > 0) {
                min_height = min(min_height, height[e.to]);
            }
        }
        if (min_height != INT_MAX) {
            height[u] = min_height + 1;
        }
    }

    void discharge(int u, int s, int t){
        while (excess[u] > 0) {
            if (seen[u] < (int)adj[u].size()) {
                Edge &e = adj[u][seen[u]];
                if (e.capacity - e.flow > 0 && height[u] == height[e.to] + 1) {
                    push(u, e);
                    if (e.to != s && e.to != t && excess[e.to] > 0) {
                        excess_nodes.push(e.to);
                    }
                } else {
                    seen[u]++;
                }
            } else {
                relabel(u);
                seen[u] = 0;
            }
        }
    }

    int max_flow(int s, int t){
        height[s] = n;
        excess[s] = INT_MAX;

        for (auto &e : adj[s]) {
            if (e.capacity > 0) {
                push(s, e);
                if (e.to != s && e.to != t && excess[e.to] > 0) {
                    excess_nodes.push(e.to);
                }
            }
        }

        while (!excess_nodes.empty()) {
            int u = excess_nodes.front();
            excess_nodes.pop();
            if (u != s && u != t) {
                discharge(u, s, t);
            }
        }

        return excess[t];
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
