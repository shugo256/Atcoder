#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
#include <string>
#include <cmath>
#include <iomanip>
#include <complex>

using ll = long long;

using namespace std;


#include <complex>
#include <numeric>
#include <iomanip>

#define X real()
#define Y imag()

// operator overload for complex<value_t>
namespace std {
    using value_t = double;

    inline bool operator<(const complex<value_t> l, const complex<value_t> r) {
        return (l.X != r.X ? l.X < r.X : l.Y < r.Y);
    }
    inline bool operator>(const complex<value_t> l, const complex<value_t> r) {
        return (l.X != r.X ? l.X > r.X : l.Y > r.Y);
    }
    inline bool operator<=(const complex<value_t> l, const complex<value_t> r) { return !(l > r); }
    inline bool operator>=(const complex<value_t> l, const complex<value_t> r) { return !(l < r); }
    istream &operator>>(istream &is, complex<value_t> &p) {
        value_t a, b;
        is >> a >> b;
        p = complex<value_t>(a, b);
        return is;
    }
    ostream &operator<<(ostream &os, const complex<value_t> &p) {
        return os << fixed << setprecision(10) << p.X << ' ' << p.Y;
    }
}


namespace geometry2d {

    using value_t = std::value_t;
    using point_t = complex<value_t>;

    const value_t EPS = 1e-10;

    // 内積, 外積, 単位ベクトル, なす角のcos
    inline value_t dot(point_t a, point_t b)   { return a.X * b.X + a.Y * b.Y; }
    inline value_t cross(point_t a, point_t b) { return a.X * b.Y - a.Y * b.X; }
    inline point_t unit(point_t p) { return p / abs(p); }
    inline value_t cos_between(point_t a, point_t b) { return dot(unit(a), unit(b)); }

    // 多角形を表す構造体
    struct polygon : public vector<point_t> {
        using vector<point_t>::vector;
        using vector<point_t>::operator[];
        using vector<point_t>::resize;
        
        polygon(int size) : vector((size_t)size) {}
        inline point_t operator[](int i) const { return (*this)[(size_t)i]; }
        inline point_t& operator[](int i) { return (*this)[(size_t)i]; }
        inline void resize(int size) { resize((size_t)size); }
        inline int size() { return (int)vector<point_t>::size(); }

        // 凸包を作る(O(nlogn))
        // ansに凸包の各点(pointsでのインデックス)が入る
        // 戻り値は凸包を示すpolygon
        polygon convex_hull(vector<int> &ans_id) {
            int n = this->size(), k = 0;
            vector<int> sorted_id((size_t)n);
            iota(sorted_id.begin(), sorted_id.end(), 0);
            sort(sorted_id.begin(), sorted_id.end(), [&](size_t l, size_t r) {
                return (*this)[l] < (*this)[r];
            });

            ans_id.resize(2 * (size_t)n);
            
            auto ans =    [&](int i) -> point_t& { return (*this)[ans_id[(size_t)i]]; };
            auto sorted = [&](int i) -> point_t& { return (*this)[sorted_id[(size_t)i]]; };

            // lower hull
            for (int i=0; i<n; i++) {
                while (k > 1 && cross(ans(k-1) - ans(k-2), sorted(i) - ans(k-1)) <= 0) k--;
                ans_id[(size_t)k] = sorted_id[(size_t)i];
                k++;
            }
            // upper hull
            for (int i=n-2, s=(int)k; i>=0; i--) {
                while ((int)k > s && cross(ans(k-1) - ans(k-2), sorted(i) - ans(k-1)) <= 0) k--;
                ans_id[(size_t)k] = sorted_id[(size_t)i];
                k++;
            }
            ans_id.resize((size_t)k-1);

            polygon ans_poly(k-1);
            for (int i=0; i<k-1; i++) ans_poly[i] = ans(i);
            return ans_poly;
        }
        // idなしver
        polygon convex_hull() {
            vector<int> dummy;
            return convex_hull(dummy);
        }
    };


    // 円を表す構造体
    struct circle {
        point_t c; // center
        value_t r; // radius

        // 点pを内部に含むなら2, 円周上なら1, 外なら0
        int contains(const point_t p) {
            value_t d = abs(p - c);
            if (d < r - EPS)
                return 2;
            else if (d < r + EPS)
                return 1;
            else
                return 0;
        }
    };

    // 交点の数
    // 完全に一致する場合は3を返す
    // TODO: c++17になったらtupleで返す?
    inline int intersects(const circle a, const circle b) {
        double dist = abs(a.c - b.c);
        double rdif = abs(a.r - b.r);
        double rsum = a.r + b.r;

        if (dist < EPS && rdif < EPS) return 3;
        else if (dist < rdif - EPS)   return 0;
        else if (dist < rdif + EPS)   return 1;
        else if (dist < rsum - EPS)   return 2;
        else if (dist < rsum + EPS)   return 1;
        else                          return 0;
    }

    // 交点
    // intersectsが0か3の時は無意味なのでfalseを返す
    inline bool intersection_point(const circle a, const circle b, point_t &p1, point_t &p2) {
        int icnt = intersects(a, b);
        if (icnt == 0 || icnt == 3) return false;

        point_t vec_ab = b.c - a.c;
        value_t d = abs(vec_ab);
        value_t theta = arg(vec_ab);
        value_t alpha = acos((a.r * a.r + d * d - b.r * b.r) / (2 * a.r * d));

        p1 = a.c + polar(a.r, theta + alpha);
        p2 = a.c + polar(a.r, theta - alpha);

        return true;
    }

    // 円たちが一番重なってる部分の枚数
    // O(n^3)
    inline int count_overlap(vector<circle> &circles) {
        int ans = 1;
        size_t n = circles.size();
        for (size_t i=0; i<n; i++) {
            for (size_t j=0; j<i; j++) {
                point_t p1, p2;
                double dist = abs(circles[i].c - circles[j].c);
                double rsum = circles[i].r + circles[j].r;      
                if (!intersection_point(circles[i], circles[j], p1, p2)) {
                    if (dist > rsum + EPS) continue; // 完全に外側
                    circle insider = (circles[i].r < circles[j].r ? circles[i] : circles[j]); // 含まれる側
                    p1 = p2 = insider.c;
                }

                int cnt1 = 0, cnt2 = 0;
                for (size_t k=0; k<n; k++) {
                    if (circles[k].contains(p1)) cnt1++;
                    if (circles[k].contains(p2)) cnt2++;
                }
                ans = max({ans, cnt1, cnt2});
            }
        }
        return ans;
    }

} // namespace geometry2d

namespace geo = geometry2d;

int main() {
    int n;
    cin >> n;
    vector<geo::circle> cs;
    for (int i=0; i<n; i++) {
        geo::point_t p;
        cin >> p;
        cs.push_back({p, 0});
    }
    double l = 0, r = 10000;
    while (r - l > geo::EPS) {
        double m = (l + r) / 2;
        for (auto &c:cs) c.r = m;
        int cnt = geo::count_overlap(cs);
        if (cnt == n) r = m;
        else l = m;
    }
    cout << setprecision(15) << fixed << l << '\n';
    return 0;
}