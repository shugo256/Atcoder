#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
#include <string>

using ll = long long;

using namespace std;

#include <functional>
#include <limits>

#define ASSIGN_I [](T &ival, T x) { ival = x; }  // i番目にxを代入(点更新のデフォルト)

template <typename T>
class SegmentTree {
    int n;                       // 葉の数
    T def;                 // 初期値 && 単位元
    vector<T> tree;              // 本体 

    using op_func_t = function<T(T, T)>;
    using up_func_t = function<void(T&, T)>;

    op_func_t op_func; // 区間クエリで使う処理
    up_func_t up_func; // 点更新で使う処理 ただの変更の他、i番目にxをたすみたいなこともできる

    // 区間[a, b)の総和(と言うか総operation(は？))
    // ノードk=[l, r)を見ている
    T _query(int a, int b, int k, int l, int r) {
        if (r <= a || b <= l) return def;            // 全く交差しない場合
        if (a <= l && r <= b) return tree[(size_t)k]; // 完全に包含される場合
        T vl = _query(a, b, k * 2 + 1, l, (l + r) / 2),
          vr = _query(a, b, k * 2 + 2, (l + r) / 2, r);
        return op_func(vl, vr);
    }

  public:
    SegmentTree(int _n, T _def, op_func_t _op_func, up_func_t _up_func = ASSIGN_I)
        : def(_def), op_func(_op_func), up_func(_up_func)  {
        n = 1;
        while (n < _n) n <<= 1;
        tree = vector<T>((size_t)(2*n - 1), def);
    }

    //よく使うやつら min, max, sum
    SegmentTree(int _n, T _def, string mode, up_func_t _up_func = ASSIGN_I)
        : SegmentTree(
            _n, 
            _def,
            mode == "max" ? [](T l, T r) { return max(l, r); } :
           (mode == "min" ? [](T l, T r) { return min(l, r); } :
                            [](T l, T r) { return l + r; }), // sum
            _up_func
        ) {}

    SegmentTree(int _n, string mode, up_func_t _up_func = ASSIGN_I)
        : SegmentTree(
            _n, 
            mode == "max" ? numeric_limits<T>::lowest() : 
           (mode == "min" ? numeric_limits<T>::max() :
                            0), // sum
            mode,
            _up_func
        ) {}

    template <typename ID>
    void update(ID i, T x) {
        i += (ID)n - 1;
        up_func(tree[(size_t)i], x);
        while (i > 0) {
            i = (i - 1) / 2;
            tree[(size_t)i] = op_func(tree[(size_t)(i * 2 + 1)], tree[(size_t)(i * 2 + 2)]);
        }
    }

    template <typename ID1, typename ID2>
    T query(ID1 a, ID2 b) { return _query((int)a, (int)b, 0, 0, n); }

    void print_tree() {
        size_t next = 0, size = (size_t)(2 * n - 1);
        for (size_t i=0; i<size; i++) {
            cout << tree[i];
            if (i == next) {
                cout << '\n';
                next = (next + 1) * 2;
            } else 
                cout << string(size * 2 / (next+2), ' ');
        }
    }

    auto begin() { return tree.begin() + n - 1; }
    auto end() { return tree.end(); }

    T operator[](int i) { return tree[ (size_t)(i + n - 1) ]; }
};
/*
コンストラクタ
SegmentTree(n, def, op_func, [up_func])
SegmentTree(n, def, mode, [up_func])
SegmentTree(n, mode, [up_func])
*/

int main() {
    int n, q;
    string s;
    cin >> n >> s >> q;
    vector<SegmentTree<int>> vsg(26, SegmentTree<int>(n, "sum"));
    for (int i=0; i<n; i++) {
        vsg[s[i] - 'a'].update(i, 1);
    }
    for (int i=0, t; i<q; i++) {
        cin >> t;
        if (t == 1) {
            int id; char c;
            cin >> id >> c;
            id--;
            vsg[s[id] - 'a'].update(id, 0);
            vsg[c - 'a'].update(id, 1);
            s[id] = c;
        }
        else {
            int l, r;
            cin >> l >> r;
            l--;
            int cnt = 0;
            for (auto &vi:vsg) {
                if (vi.query(l, r)) cnt++;
            }
            cout << cnt << '\n';
        }
    }
    return 0;
}