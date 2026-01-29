#include <bits/stdc++.h>

using namespace std;

int main(){

    int n, m, k;
    scanf("%d%d%d", &n, &m, &k);

    int mx = 0;
    vector<int> need(n+1);
    for (int i = 1; i <= n; ++i) {
        scanf("%d", &need[i]);
        mx = max(mx, need[i]);
    }

    vector<vector<int>> graph(n + 1);
    vector<int> in_deg(n + 1);
    for (int i = 0; i < m; ++i){
        int a, b;
        scanf("%d%d", &a, &b);
        graph[b].push_back(a);
        in_deg[a]++;
    }

    int result = 1e9;
    int l = 1;
    int r = mx;
    while (l <= r) {
        int p = (l + r) >> 1;
        vector<int> tmp_deg = in_deg;
        queue<int> q;
        for (int i = 1; i <= n; ++i) {
            if (tmp_deg[i] == 0)
                q.push(i);
        }

        int done = 0;
        while (!q.empty()) {
            int x = q.front();
            
            if (need[x] <= p) {
                ++done;
                for (int y : graph[x]) {
                    --tmp_deg[y];
                    if (!tmp_deg[y]) {
                        q.push(y);
                    }
                }
            }
            q.pop();
        }

        if (done >= k) {
            result = min(result, p);
            r = p - 1;
        } else {
            l = p + 1;
        }
    }

    printf("%d\n", result);
    return 0;
}