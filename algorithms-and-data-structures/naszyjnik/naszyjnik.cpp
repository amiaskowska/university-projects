#include <bits/stdc++.h>

using namespace std;

int main() { 

    int n, k;
    scanf("%d %d", &n, &k);

    vector<long long> t(n + 1);
    for(int i = 1; i <= n; ++i) 
        scanf("%lld", &t[i]);

    // dp[prefiks][dlugosc_obecnego_przedzialu][czy_k_wziety] = max wynik
    vector<vector<vector<long long>>> dp(n + 1);
    for(int i = 0; i <= n; ++i) {
        dp[i].resize(k + 1);
        for(int j = 0; j <= k; ++j){
            dp[i][j].resize(2);
            dp[i][j][0] = -1e18;
            dp[i][j][1] = -1e18;
        }
    }

    dp[0][0][0] = 0;
    for(int i = 1; i <= n; ++i) {
        for(int j = 0; j < k; ++j) {
            if(j > i) 
                break;
            dp[i][0][0] = max(dp[i][0][0], dp[i - 1][j][0]);
            dp[i][0][1] = max(dp[i][0][1], dp[i - 1][j][1]); 
        }
        dp[i][0][1] = max(dp[i][0][1], dp[i - 1][k][1]);

        for(int j = 1; j < k; ++j) {
            if(j > i)
                break;
            dp[i][j][0] = max(dp[i][j][0], dp[i - 1][j - 1][0] + t[i]);
            dp[i][j][1] = max(dp[i][j][1], dp[i - 1][j - 1][1] + t[i]);
        }
       
        if(k > i)
            continue;
        dp[i][k][1] = max(dp[i][k][1], dp[i - 1][k - 1][0] + t[i]);
    }

    long long res = 0;
    for(int i = 0; i <= k; ++i) {
        res = max(res, max(dp[n][i][1], dp[n][i][0]));
    }
    printf("%lld", res);
    return 0;
}