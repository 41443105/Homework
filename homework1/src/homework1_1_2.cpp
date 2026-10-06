#include <iostream>
using namespace std;

int a(int m, int n) {
    // 設定一個足夠大的 s 來模擬遞迴
    const int max = 100000; // 可依需求調整
    int s[max];
    int top = 0;
    s[top++] = m;

    while (top > 0) {
        m = s[--top];
        if (m == 0) {
            n = n + 1;
        }
        else if (n == 0) {
            s[top++] = m - 1;
            n = 1;
        }
        else {
            s[top++] = m - 1;
            s[top++] = m;
            n = n - 1;
        }
    }
    return n;
}
int main() {
    int m, n;
    cout << "請輸入m和n :";
    cin >> m >> n;
    cout << a(m, n) << endl;
    return 0;
}