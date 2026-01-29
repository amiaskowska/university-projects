#include <bits/stdc++.h>

using namespace std;

struct seg {
    int N = 1;
    vector<int> t;

    seg(int n, vector<int> &vl) {
        while(N < n) N <<= 1;
        t.resize(N << 1, 1e9);
        
        for (int i = 0; i < n; ++i) t[i + N] = vl[i];
        for (int i = N - 1; i; --i) t[i] = min(t[i * 2], t[i * 2 + 1]);
    }

    int query (int x, int y) {
        int result = 1e9;
        for (x += N - 1, y += N; x < y; x >>= 1, y>>= 1) {
            if (x&1) result = min(result, t[x]), ++x;
            if (y&1) --y, result = min(result, t[y]);
        }

        return result;
    }
};

void dfs (int x, vector<int> &seg, vector<int> &vis, vector<int> &preorder, vector<int> &seg_ind,
        int &num, vector<vector<int>> &graph, vector<int>& depth, int d) {
    preorder[x] = num;
    ++num;
    depth[x] = d;
    seg.emplace_back(depth[x]);
    seg_ind[x] = seg.size();
    vis[x] = 1;
    
    for(int y: graph[x]) {
        if (vis[y]) 
            continue;
        dfs(y, seg, vis, preorder, seg_ind, num, graph, depth, d + 1);
        seg.emplace_back(depth[x]);
    }
}

int main() {
    int n, q;
    scanf("%d%d", &n, &q);

    vector<vector<int>> graph(n + 1);
    for (int i = 0; i < n - 1; ++i) {
        int a, b;
        scanf("%d%d", &a, &b);

        graph[a].emplace_back(b);
        graph[b].emplace_back(a);
    }

    int num = 0;
    vector<int> preorder(n + 1), seq, seg_ind(n+1), vis(n + 1), depth(n + 1);
    dfs(1, seq, vis, preorder, seg_ind, num, graph, depth, 0);

    int s  = seq.size();
    seg T(s, seq);

    for (int i = 0; i < q; ++i){
        int a, b, c;
        scanf("%d%d%d", &a, &b, &c);

        vector<pair<int, int>> t;
        t.emplace_back(preorder[a], a);
        t.emplace_back(preorder[b], b);
        t.emplace_back(preorder[c], c);
        sort(t.begin(), t.end());

        int result = 0;
        int x1 = seg_ind[t[0].second];
        int x2 = seg_ind[t[1].second];
        int x3 = seg_ind[t[2].second];
        result += 2 * depth[t[0].second] + 2 * depth[t[1].second] + 2 * depth[t[2].second];
        result -= 2 * T.query(x1, x2) + 2 * T.query(x2, x3) + 2 * T.query(x1, x3);
        
        printf("%d\n", (result >> 1));
    }
    return 0;
}