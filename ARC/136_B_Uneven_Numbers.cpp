#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
#include <string>

using namespace std;

int main() {
    int n;
    cin >> n;
    if (n < 10)
        cout << n << '\n';
    else if (n < 100)
        cout << 9 << '\n';
    else if (n < 1000)
        cout << 9 + n - 99 << '\n';
    else if (n < 10000)
        cout << 909 << '\n';
    else if (n < 100000) 
        cout << 909 + n - 9999 << '\n';
    else 
        cout << 90909 << endl;
    return 0;
}