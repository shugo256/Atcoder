#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
#include <string>

using ll = long long;

using namespace std;

int main() {
    int l, r, d;
    cin >> l >> r >> d;
    int ans = 0;
    for (int i=l; i<=r; i++) {
        if (i % d == 0) ans++;
    }
    cout << ans << '\n';
    return 0;
}