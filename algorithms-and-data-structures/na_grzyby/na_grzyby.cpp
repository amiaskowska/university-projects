#include <bits/stdc++.h>

using namespace std;

const int mod = 1e9;
int add(int a, int b) {
    int s = a + b;
    if(s >= mod)
        s -= mod;
    return s;
}
int main(){

    int n, m, k, g;
    scanf("%d %d %d %d", &n, &m, &k, &g);

    vector<vector<vector<vector<int>>>> r(n + 1);
    vector<vector<int>> t(n + 1);

    // r[i][j][k][0/1/2] = liczba ścieżek do miejsca (i,j), które
    // zbierają co najmniej k grzybów (0 jeśli Bajtazar przyszedł z lewej,
    // 1 - z góry i 2 - z dołu).
    // Kolumny zredukowane do dwóch (potrzebne odowłanie tylko do poprzedniej).
    for(int i = 0; i <= n; ++i){
        r[i].resize(2);
        for(int j = 0; j <= 1; ++j){
            r[i][j].resize(k + 1);
            for(int p = 0; p <= k; ++p) 
                r[i][j][p].resize(3);
        }
    }

    for(int i = 0; i <= n; ++i) 
        t[i].resize(m + 1);


    int a, b;
    for(int i = 0; i < g; ++i) {
        scanf("%d %d", &a, &b);
        t[a][b] = 1;
    }

    if(t[1][1]) 
        r[1][1][1][0] = 1;
    else 
        r[1][1][0][0] = 1; 
    int x = 1;
    for(int j = 1; j <= m; ++j){ // j - numer obecnej kolumny
    
        // Rozpatrujemy przejście z z lewej oraz przejście z góry.
        for(int i = 1; i <= n; ++i){ // i - numer obecnego wiersza
            for(int p = 0; p <= k; ++p){ // p - liczba zebranych grzybów do poprzedniego pola.
                int my_p = p;
                if(t[i][j]) ++my_p;
                my_p = min(my_p, k);

                r[i][x][my_p][0] = add(r[i][x][my_p][0], r[i][x^1][p][0]); // 1
                r[i][x][my_p][0] = add(r[i][x][my_p][0], r[i][x^1][p][1]); // 2
                r[i][x][my_p][0] = add(r[i][x][my_p][0], r[i][x^1][p][2]); // 3

                r[i][x][my_p][1] = add(r[i][x][my_p][1], r[i - 1][x][p][0]); // 7
                r[i][x][my_p][1] = add(r[i][x][my_p][1], r[i - 1][x][p][1]); // 5
            }
        }

        // Rozpatrujemy przejście z dołu.
        for(int i = n - 1; i >= 1; --i){ // i - numer obecnego wiersza 
            for(int p = 0; p <= k; ++p){ // p - liczba zebranych grzybów do poprzedniego pola.
                int my_p = p;
                if(t[i][j]) ++my_p;
                my_p = min(my_p, k);

                r[i][x][my_p][2] = add(r[i][x][my_p][2], r[i + 1][x][p][0]); // 6
                r[i][x][my_p][2] = add(r[i][x][my_p][2], r[i + 1][x][p][2]); // 4
            }
        }

        for(int i = 1; i <= n; ++i){ 
            for(int p = 0; p <= k; ++p){ 
                r[i][x^1][p][0] = 0;
                r[i][x^1][p][1] = 0;
                r[i][x^1][p][2] = 0;
            }
        }

        x^= 1;
    }

    int res = add(r[n][x^1][k][0], r[n][x^1][k][1]);
    printf("%d\n", res);
    return 0;
}