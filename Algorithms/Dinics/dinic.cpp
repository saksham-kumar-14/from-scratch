#include<bits/stdc++.h>
using namespace std;

struct Edge{
    int v;              // u->v, v: to node
    int flow;           // flow in the edge
    int capacity;       // maximum flow capacity
    int rev;            // index of reverse edge in adj[v]
};

// Residual Graph
class Graph{
    int V;
    vector<int> level;
    vector<int> iter;
    vector<vector<Edge>> adj;

public:
    Graph(int n){
        V = n;
        adj.assign(n, {});
        level.assign(n, -1);
        iter.assign(n, 0);
    }

    void add(int u, int v, int cap){
        Edge forward_edge = {
            v, 0, cap, static_cast<int>(adj[v].size())
        };
        Edge back_edge = {
            u, 0, 0,static_cast<int>(adj[u].size())
        };
        adj[u].push_back(forward_edge);
        adj[v].push_back(back_edge);
    }

    bool bfs(int s, int t){
        fill(level.begin(), level.end(), -1);
        queue<int> q;
        level[s] = 0;
        q.push(s);
        while(!q.empty()){
            int u = q.front();
            q.pop();
            for(auto edge: adj[u]){
                if(level[edge.v] == -1 and edge.flow < edge.capacity){
                    level[edge.v] = level[u] + 1;
                    q.push(edge.v);
                }
            }
        }

        return level[t] >= 0;
    }

    // DFS to find the blocking flow in O(n * m)
    int dfs(int u, int t, int pushed){
        if(u == t) return pushed;
        for(int &i = iter[u]; i < adj[u].size(); ++i){
            Edge &edge = adj[u][i];
            if(level[edge.v] == level[u] + 1 and edge.flow < edge.capacity){
                int d = dfs(edge.v, t, min(pushed, edge.capacity - edge.flow));
                if(d > 0){
                    edge.flow += d;
                    adj[edge.v][edge.rev].flow -= d;
                    return d;
                }
            }
        }
        return 0;
    }

    int max_flow(int s, int t){
        int flow = 0;
        while(bfs(s,t)){
            fill(iter.begin(), iter.end(), 0);
            int pushed = dfs(s, t, INT_MAX);
            while(pushed > 0){
                flow += pushed;
                pushed = dfs(s, t, INT_MAX);
            }
        }

        return flow;
    }
};

int main(){
    int n, m;
    cin >> n >> m;

    Graph g(n);
    for(int i = 0; i < m; ++i){
        int u, v, cap;
        cin >> u >> v >> cap;
        g.add(u, v, cap);
    }

    int s, t;
    cin >> s >> t;

    cout << g.max_flow(s, t) << '\n';

    return 0;
}
