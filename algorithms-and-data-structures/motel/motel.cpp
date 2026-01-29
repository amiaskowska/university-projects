#include <bits/stdc++.h>

using namespace std;

void dijkstra(int start, vector<long long>& d, vector<vector<pair<int,int>>>& v) {
    priority_queue<pair<long long, int>> pq;
    d[start] = 0;
    pq.push({0, start});
    while (!pq.empty()) {
        int x = pq.top().second;
        long long dist = -pq.top().first;
        pq.pop();

        for (int i = 0; i < (int)v[x].size(); ++i) {
            if ((dist + (long long)v[x][i].second) < d[v[x][i].first]) {
                d[v[x][i].first] = dist + (long long)v[x][i].second;
                pq.push({-(d[v[x][i].first]), v[x][i].first});
            }
        }
    }
}

int main(){
    int n, m;
    scanf("%d %d", &n, &m);

    vector<vector<pair<int, int>>> v(n + 1);
    vector<vector<pair<int, int>>> rev_v(n + 1);
    for (int i = 0; i < m; ++i) {
        int a, b, c;
        scanf("%d%d%d", &a, &b, &c);
        v[a].push_back({b, c});
        rev_v[b].push_back({a, c});
    }

    vector<long long> d1(n + 1, 1e18), d2(n + 1, 1e18);
    dijkstra(0, d1, v);
    dijkstra(n - 1, d2, rev_v);

    long long min_longer = 1e18;
    for (int i = 0; i < n; ++i) {
        long long longer = max(d1[i], d2[i]);
        min_longer = min(min_longer, longer);
    }

    printf("%lld\n", min_longer);

    return 0;
}