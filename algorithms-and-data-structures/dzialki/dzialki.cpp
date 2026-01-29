#include <bits/stdc++.h>

using namespace std;

struct seg {
    int N = 1;
    vector<int> t;

    void init(int n) {
        while (N < n) 
            N <<= 1;
        t.resize(N * 2 + 1);
    }

    void insert(int x) {
        int i = x + N - 1;
        while (i) {
            t[i]++;
            i >>= 1;
        }
    }

    int query(int i, int bg, int en, int x, int y) {
        if(x <= bg && en <= y) 
            return t[i];
        
        int res = 0;
        int mid = (bg + en) >> 1;
        if(x <= mid) res += query(i * 2, bg, mid, x, y);
        if(y > mid) res += query(i * 2 + 1, mid + 1, en, x, y);
        
        return res;
    }
    
};

struct rect_sorted {
    int x, bottom, top, i;
    bool isLeft;

    bool operator< (const rect_sorted& other) const {
        if (x != other.x) 
            return x < other.x;
        return isLeft > other.isLeft;
    }
};

struct point {
    int x, y;

    bool operator <(const point& other) const {
        if(x != other.x) 
            return x < other.x;
        return y < other.y;
    }
};
int main(){

    int n, m;
    scanf("%d%d", &n, &m);

    set<int> s;
    vector<point> p(n);
    for (int i = 0; i < n; ++i){
        scanf("%d %d", &p[i].x, &p[i].y);
        s.insert(p[i].y);
    }

    vector<rect_sorted> r;
    for (int i = 0; i < m; ++i) {
        int x1, x2, y1, y2;
        scanf("%d%d%d%d", &x1, &x2, &y1, &y2);
        r.push_back({x1, y1, y2, i, true});
        r.push_back({x2, y1, y2, i, false});
        s.insert(y1);
        s.insert(y2);
    }

    sort(p.begin(), p.end());
    sort(r.begin(), r.end());

    unordered_map<int, int> t;
    int num = 1;
    for (int it: s) {
        t[it] = num;
        ++num;
    }


    seg T; 
    T.init(num);
    vector<int> res(m);
    int it = 0;
    for (int i = 0; i < m * 2; ++i) {
        if (r[i].isLeft){
            while (it < n && p[it].x < r[i].x) {
                T.insert(t[p[it].y]);
                ++it;
            }
            res[r[i].i] -= T.query(1, 1, T.N, t[r[i].bottom], t[r[i].top]);
        } else {
            while (it < n && p[it].x <= r[i].x) {
                T.insert(t[p[it].y]);
                ++it;
            }
            res[r[i].i] += T.query(1, 1, T.N, t[r[i].bottom], t[r[i].top]);
        }
        
    }

    for (int i = 0; i < m; ++i) 
        printf("%d\n", res[i]);
    return 0;
}