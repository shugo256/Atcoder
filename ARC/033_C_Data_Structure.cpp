#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
#include <string>

using ll = long long;

using namespace std;

// 和以外はseg木でよくねという発想の元、和のみを実装
template <typename T>
class BIT {
    const int n;
    vector<T> tree;

public:
    BIT(int _n) : n(_n), tree((size_t)_n, 0) {}

    // v[a] += w
    template <typename ID>
    void add(ID a, T w) {
        for (int i = (int)a; i < n; i |= (i + 1)) 
            tree[(size_t)i] += w;
    }

    // v[0] + v[1] + ... + v[a-1]
    template <typename ID>
    T sum(ID a) {
        T res = 0;
        for (int i = (int)a - 1; i >= 0; i = (i & (i + 1)) - 1)
            res += tree[(size_t)i];
        return res;
    }

    // v[0] + v[1] + ... + v[a] >= w なる最小のa
    int lower_bound(T w) {
        int x = 0, beki = 1;
        for (int _n=n; _n>0; _n>>=1) beki <<= 1;
        for (int k=beki; k>0; k>>=1) {
            if (x + k <= n && tree[(size_t)(x + k - 1)] < w) {
                w -= tree[(size_t)(x + k - 1)];
                x += k;
            }
            //cerr << k << ' ' << x << ' ' << tree[x] << '\n';
        }
        return x;
    }

    // set(被りなし)としての機能を見やすく
    // ただし同じものをたとえば二回pushする場合、値が+2となって一回のpopでは消えないというバグあり
    template <typename ID>
    void push(ID a) { add(a, 1); }

    template <typename ID>
    void pop(ID a) { add(a, -1); }
};

#define MAX 200010

int main() {
    int q;
    cin >> q;
    BIT<int> bit(MAX);
    for (int i=0; i<q; i++) {
        int t, x; cin >> t >> x;
        if (t == 1) bit.push(x);
        else {
            int lb = bit.lower_bound(x);
            cout << lb << '\n';
            bit.pop(lb);
        }
    }
    return 0;
}