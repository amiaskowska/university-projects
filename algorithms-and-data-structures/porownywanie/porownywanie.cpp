#include <bits/stdc++.h>
typedef long long ll;

struct hash{
    const ll p1 = 31, p2 = 37;
    const ll mod = 1e9+7;
    std::vector<ll> pp1, pp2, rp1, rp2;
    std::vector<std::pair<ll,ll>> vl;

    void init(int n) {
        pp1.push_back(1);
        pp2.push_back(1);
        vl.resize(n + 1);

        for (int i = 1; i <= n; ++i) {
            pp1.push_back((pp1.back() * p1) % mod);
            pp2.push_back((pp2.back() * p2) % mod);
            rp1.push_back(pow(pp1.back(), mod - 2));
            rp2.push_back(pow(pp2.back(), mod - 2));
        }
    }

    void new_hash(int i, long long s) {
        vl[i] = {(vl[i - 1].first + s * pp1[i]) % mod, 
                (vl[i - 1].second + s * pp2[i]) % mod};
    }

    bool comp(int a, int b, int c, int d) {
        std::pair<ll, ll> A, B;
        A = {vl[b].first + mod - vl[a - 1].first, vl[b].second + mod - vl[a - 1].second};
        A.first = (A.first * rp1[a]) % mod;
        A.second = (A.second * rp2[a]) % mod;

        B = {vl[d].first + mod - vl[c - 1].first, vl[d].second + mod - vl[c - 1].second};
        B.first = (B.first * rp1[c]) % mod;
        B.second = (B.second * rp2[c]) % mod;

        return A == B;
    }

    ll pow(ll a, ll p) {
        if (!p) return 1;
        else if(p & 1) 
            return (a * pow(a, --p)) % mod;
        else {
            ll ret = pow(a, p >> 1);
            return (ret * ret) % mod;
        }
    }
};

int first_off(int a, int c, int len, hash& h) {
    int bg = 0, en = len - 1;
    int ret = len;
    while (bg <= en) {
        int mid = (bg + en) >> 1;
        if (h.comp(a, a + mid, c, c + mid)) {
            bg = mid + 1;
        }
        else {
            ret = std::min(ret, mid);
            en = mid - 1;
        }
    }

    return ret;
}

int main() {
    std::ios_base::sync_with_stdio(0);
    std::cin.tie(0);
    
    int n, m;
    std::cin >> n >> m;

    std::string s;
    std::cin >> s;
    
    hash h;
    h.init(n);
    for (int i = 0; i < n; ++i) {
        h.new_hash(i + 1, s[i] - 'a' + 1);
    }

    for (int i = 0; i < m; ++i) {
        int a, b, c, d;
        std::cin >> a >> b >> c >> d;

        int len1 = b - a + 1, len2 = d - c + 1;
        int diff = abs(len1 - len2);

        if (diff > 1) {
            printf("0\n");
        }
        else if (diff == 1) {

            int first = first_off(a, c, std::min(len1, len2), h);
            if (len1 > len2) {
                if (first == len2 || h.comp(a + first + 1, b, c + first, d)) 
                    printf("1\n");
                else 
                    printf("0\n");
            } else {
                if (first == len1 || h.comp(a + first, b, c + first + 1, d)) 
                    printf("1\n");
                else 
                    printf("0\n");
            }
        }
        else {

            int first = first_off(a, c, len1, h);
            if (first >= len1 - 1 || h.comp(a + first + 1, b, c + first + 1, d)) 
                printf("1\n");
            else 
                printf("0\n");
        }

    }
}