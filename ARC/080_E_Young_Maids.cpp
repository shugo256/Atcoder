#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
#include <string>

using ll = long long;

using namespace std;

struct seq{int l, r; bool bit;};

#include <functional>
#include <limits>

// i番目をxにかえるだけの関数(点更新のデフォルト) (i++はwarnig回避)
#define CHANGE_I [](int i, T x) { i++; return x; }
// i番目にxたす
#define ADD(sg) [&](int i, T x) { return sg[i] + x; }

template <typename T>
class SegmentTree {
    int n;                       // 葉の数
    T def;                 // 初期値 && 単位元
    vector<T> tree;              // 本体 
    function<T(T, T)> operation_f; // 区間クエリで使う処理
    function<T(int, T)> update_f;  // 点更新で使う処理 ただの変更の他、i番目にxをたすみたいなこともできる

    // 区間[a, b)の総和(と言うか総operation(は？))
    // ノードk=[l, r)を見ている
    T _query(int a, int b, int k, int l, int r) {
        if (r <= a || b <= l) return def;            // 全く交差しない場合
        if (a <= l && r <= b) return tree[(size_t)k]; // 完全に包含される場合
        T vl = _query(a, b, k * 2 + 1, l, (l + r) / 2),
          vr = _query(a, b, k * 2 + 2, (l + r) / 2, r);
        return operation_f(vl, vr);
    }

  public:
    SegmentTree(int _n, T _def, function<T(T, T)> _operation_f, function<T(int, T)> _update_f = CHANGE_I)
        : def(_def), operation_f(_operation_f), update_f(_update_f)  {
        n = 1;
        while (n < _n) n <<= 1;
        tree = vector<T>((size_t)(2*n - 1), def);
    }

    //よく使うやつら min, max, sum
    SegmentTree(int _n, T _def, string mode, function<T(int, T)> _update_f = CHANGE_I)
        : SegmentTree(
            _n, 
            _def,
            mode == "max" ? [](T l, T r) { return max(l, r); } :
           (mode == "min" ? [](T l, T r) { return min(l, r); } :
                            [](T l, T r) { return l + r; }), // sum
            _update_f
        ) {}

    SegmentTree(int _n, string mode, function<T(int, T)> _update_f = CHANGE_I)
        : SegmentTree(
            _n, 
            mode == "max" ? numeric_limits<T>::lowest() : 
           (mode == "min" ? numeric_limits<T>::max() :
                            0), // sum
            mode,
            _update_f
        ) {}

    template <typename ID>
    void update(ID i, T x) {
        T next_i = update_f((int)i, x);
        i += (ID)n - 1;
        tree[(size_t)i] = next_i;
        while (i > 0) {
            i = (i - 1) / 2;
            tree[(size_t)i] = operation_f(tree[(size_t)(i * 2 + 1)], tree[(size_t)(i * 2 + 2)]);
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

    typename vector<T>::iterator begin() { return tree.begin() + n - 1; }
    typename vector<T>::iterator end() { return tree.end(); }

    T operator[](int i) { return tree[ (size_t)(i + n - 1) ]; }
};
/*
コンストラクタ
SegmentTree(n, def, op_func, [up_func])
SegmentTree(n, def, mode, [up_func])
SegmentTree(n, mode, [up_func])
*/


int main() {
    int n;
    cin >> n;
    int pinv[n];
    vector<int> ans;
    vector<SegmentTree<int>> sgt(2, SegmentTree<int>(n, "min"));
    for (int i=0, p; i<n; i++) {
        cin >> p;
        pinv[p] = i;
        sgt[i%2].update(i, p);
    }
    priority_queue<seq, vector<seq>, function<bool(seq, seq)>> pq([&](seq a, seq b){
        return sgt[0 ^ a.bit].query(a.l, a.r) > sgt[0 ^ b.bit].query(b.l, b.r);
    });
    pq.push({0, n, false});
    while (!pq.empty()) {
        seq t = pq.top(); pq.pop();
        int one = sgt[0 ^ t.bit].query(t.l, t.r),
            two = sgt[1 ^ t.bit].query(pinv[one]+1, t.r);
        ans.push_back(one);
        ans.push_back(two);
        if (t.l < pinv[one]) pq.push({t.l, pinv[one], t.bit});
        if (pinv[one]+1 < pinv[two]) pq.push({pinv[one]+1, pinv[two], !t.bit});
        if (pinv[two]+1 < t.r) pq.push({pinv[two]+1, t.r, t.bit});
    }
    for (auto &ai:ans) cout << ai << ' ';
    cout << '\n';
    return 0;
}