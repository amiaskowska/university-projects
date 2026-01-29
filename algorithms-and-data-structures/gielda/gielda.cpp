#include <bits/stdc++.h>

using namespace std;

struct seg {
    vector<int> s, m;
    int N = 1;

    void init(int n) {
        while (N < n) N <<= 1;
        s.resize(N * 2 + 1, 0);
        m.resize(N * 2 + 1, 1e9);
    }

    void ins(int x) {
        int i = x + N - 1;
        while (i) {
            s[i]++;
            i >>= 1;
        }
    }

    void ins_min(int x, int ind) {
        int i = x + N - 1;
        while (i) {
            m[i] = min(m[i], ind);
            i >>= 1;
        }
    }

    int query_sum(int i, int bg, int en, int x, int y) {
        if (x <= bg && en <= y) 
            return s[i];
        
        int mid = (bg + en) >> 1;
        int res = 0;
        if (x <= mid) res += query_sum(i * 2, bg, mid, x, y);
        if (y > mid) res += query_sum(i * 2 + 1, mid + 1, en, x, y);

        return res;
    }

    int query_min(int i, int bg, int en, int x, int y) {
        if (x <= bg && en <= y) 
            return m[i];

        int mid = (bg + en) >> 1;
        int res = 1e9;
        if (x <= mid) res = min(res, query_min(i * 2, bg, mid, x, y));
        if (y > mid) res = min(res, query_min(i * 2 + 1, mid + 1, en, x, y));
    
        return res;
    }

    void clear(int i) {
        s[i] = 0; m[i] = 1e9;
        if(i * 2 <= (N<<1) && s[i * 2]) clear(i * 2);
        if((i * 2 + 1) <= (N<<1) && s[i * 2 + 1]) clear(i * 2 + 1);
    }

};

struct q {
    int l, r, a, b, i;
    q() : l(0), r(0), a(0), b(0), i(0){}
    q (int l, int r, int a, int b, int i) :
        l(l), r(r), a(a), b(b), i(i) {}

    static bool comp1(const q &a , const q &b) {
        return a.r < b.r;
    }

    static bool comp2(const q &a, const q &b) {
        return a.l > b.l;
    }
};

struct res {
    int a, b;
    res() : a(0), b(1e9){}
};


void calculate_p(int l, int r, vector<int> &v, vector<q> &t, vector<res> &result, seg &ST) {
    int it_v = l;
    for (int i = 0; i < (int)t.size(); ++i){

        while (it_v <= r && it_v <= t[i].r) {
            ST.ins(v[it_v]);
            ST.ins_min(v[it_v], it_v);
            ++it_v;
        }

        result[t[i].i].a += ST.query_sum(1, 1, ST.N, t[i].a, t[i].b);
        result[t[i].i].b = min(result[t[i].i].b, ST.query_min(1,1,ST.N,t[i].a,t[i].b));

    }
}

void calculate_s(int l, int r, vector<int> &v, vector<q> &t, vector<res> &result, seg &ST) {
    int it_v = r;
    for (int i = 0; i < (int)t.size(); ++i) {
        while(it_v >= l && it_v >= t[i].l) {
            ST.ins(v[it_v]);
            ST.ins_min(v[it_v], it_v);
            --it_v;
        }
        result[t[i].i].a += ST.query_sum(1, 1, ST.N, t[i].a, t[i].b);
        result[t[i].i].b = min(result[t[i].i].b, ST.query_min(1,1,ST.N,t[i].a,t[i].b));


    }
}

void split(int l, int r, vector<int> &v, vector<q> &t, vector<res> &result, seg& ST) {
    int mid = (l + r) >> 1;

    vector<q> pr, su; 
    vector<q> queriesLeft, queriesRight;
    for (size_t i = 0; i < t.size(); ++i) {
        if(t[i].r < mid)
            queriesLeft.push_back(t[i]);
        else if(t[i].l > mid)
            queriesRight.push_back(t[i]);
        else {
            pr.push_back({mid, t[i].r, t[i].a, t[i].b, t[i].i});
            if (t[i].l < mid) su.push_back({t[i].l, mid - 1, t[i].a, t[i].b, t[i].i});
        }
        
    }

    sort(pr.begin(), pr.end(), q::comp1);
    sort(su.begin(), su.end(), q::comp2);

    calculate_p(mid, r, v, pr, result, ST);
    ST.clear(1);
    calculate_s(l, mid - 1, v, su, result, ST);
    ST.clear(1);

    if (l == r) 
        return;

    split(l, mid, v, queriesLeft, result, ST);
    split(mid + 1, r, v, queriesRight, result, ST);
}
int main() {
    int n, m;
    scanf("%d%d", &n, &m);

    set<int> s;
    vector<int> v(n + 1);
    for (int i = 1; i <= n; ++i) {
        scanf("%d", &v[i]);
        s.insert(v[i]);
    }

    vector<q> t(m);
    for (int i = 0; i < m; ++i) {
        scanf("%d%d%d%d", &t[i].l, &t[i].r, &t[i].a, &t[i].b);   
        s.insert(t[i].a);
        s.insert(t[i].b);
        t[i].i = i;
    }

    int new_vl = 1;
    unordered_map<int, int> mp;
    for (int vl : s) {
        mp[vl] = new_vl;
        ++new_vl;
    }

    for (int i = 1; i <= n; ++i) v[i] = mp[v[i]];
    for (int i = 0; i < m; ++i) {
        t[i].a = mp[t[i].a];
        t[i].b = mp[t[i].b];
    }

    vector<res> r(m);
    seg ST;
    ST.init(new_vl);
    split(1, n, v, t, r, ST);

    for (int i = 0; i < m; ++i) {
        if (r[i].b == 1e9) 
            printf("-1 ");
        else 
            printf("%d ", r[i].b);
        printf("%d\n", r[i].a);
    }

    return 0;
}