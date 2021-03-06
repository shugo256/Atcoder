#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
#include <string>
#include <set>

using ll = long long;

using namespace std;

#include <functional>
#include <limits>
#include <unordered_map>

#define ASSIGN_I [](elem_t &ival, elem_t x) { ival = x; }  // i番目にxを代入(点更新のデフォルト)

template <
    typename elem_t, 
    typename op_func_t = function<elem_t(elem_t, elem_t)>, 
    typename up_func_t = function<void(elem_t&, elem_t)>
>
class SegmentTree {
    size_t n;            // 葉の数(指定した要素数以上で最小の2べき)
    const elem_t def;    // 初期値 && 単位元
    vector<elem_t> tree; // 本体 

    op_func_t op_func;   // 区間クエリで使う処理
    up_func_t up_func;   // 点更新で使う処理 ただの変更の他、i番目にxをたすみたいなこともできる

    // stringでモード指定できるように
    // TODO: Change here to 'inline static' as soon as C++17 become available in AtCoder (and AOJ).
    //       I personally don't like initializing static vars outside.
    static unordered_map<string, elem_t> def_map;
    static unordered_map<string, op_func_t> oper_map;

  public:
    SegmentTree(int _n, elem_t _def, op_func_t _op_func, up_func_t _up_func = ASSIGN_I)
        : def(_def), op_func(_op_func), up_func(_up_func)  {
        n = 1;
        while (n < (size_t)_n) n <<= 1;
        tree = vector<elem_t>(n*2, def);
    }

    SegmentTree(int _n, elem_t _def, string mode, up_func_t _up_func = ASSIGN_I)
        : SegmentTree(_n, _def, oper_map[mode], _up_func) {}

    SegmentTree(int _n, string mode, up_func_t _up_func = ASSIGN_I)
        : SegmentTree(_n, def_map[mode], oper_map[mode], _up_func) {}

    inline void update(int _i, elem_t x) {
        size_t i = (size_t)_i;
        i += n;
        up_func(tree[i], x);
        while (i /= 2) {
            tree[i] = op_func(tree[(i * 2)], tree[(i * 2 + 1)]);
        }
    }

    // 区間[l, r)の総和(と言うか総operation(は？))
    inline elem_t query(int _l, int _r) {
        size_t l = (size_t)_l, r = (size_t)_r;
        elem_t res_l = def, res_r = def;
        for (l += n, r += n; l < r; l >>= 1, r >>= 1) {
            if (l & 1) res_l = op_func(res_l, tree[l++]);
            if (r & 1) res_r = op_func(tree[--r], res_r);
        }
        return op_func(res_l, res_r);
    }

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

    auto begin() { return tree.begin() + n; }
    auto end() { return tree.end(); }

    elem_t operator[](int i) { return tree[ (size_t)(i + n) ]; }
};

template<typename elem_t, typename op_func_t, typename up_func_t> 
unordered_map<string, elem_t> SegmentTree<elem_t, op_func_t, up_func_t>::def_map{
    {"max", numeric_limits<elem_t>::lowest()},
    {"min", numeric_limits<elem_t>::max()},
    {"sum", static_cast<elem_t>(0)}
};

template<typename elem_t, typename op_func_t, typename up_func_t> 
unordered_map<string, op_func_t> SegmentTree<elem_t, op_func_t, up_func_t>::oper_map{
    {"max", [](elem_t l, elem_t r) { return max(l, r); }},
    {"min", [](elem_t l, elem_t r) { return min(l, r); }},
    {"sum", [](elem_t l, elem_t r) { return l + r; }}
};
/*
コンストラクタ
SegmentTree(n, def, op_func, [up_func])
SegmentTree(n, def, mode, [up_func])
SegmentTree(n, mode, [up_func])
*/

#define INF 100000000

int main() {
    int h, w;
    cin >> h >> w;
    set<int> st;
    SegmentTree<int> sgt(w, INF, "min");
    for (int i=0; i<w; i++) {
        st.insert(i);
        sgt.update(i, 0);
    }
    for (int k=1, ai, bi; k<=h; k++) {
        cin >> ai >> bi;
        ai--; bi--;
        auto l = st.lower_bound(ai);
        auto r = st.upper_bound(bi+1);
        if (l != r && bi+1 < w) {
            auto itr = r;
            int prev_x = *(--itr);
            sgt.update(bi+1, sgt[prev_x] + (bi+1 - prev_x));
            st.insert(bi+1);
        }
        l = st.lower_bound(ai);
        for (auto itr=l; *itr<=bi && itr!=st.end(); itr = st.erase(itr)) {
            sgt.update(*itr, INF);
        }
        int res = k + sgt.query(0, w);
        cout << (res < INF ? res : -1) << '\n';
    }
    return 0;
}