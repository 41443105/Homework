#include <iostream>
using namespace std;
int a(int m, int n) {
    if (m == 0) return n + 1; //if m=0,n+1
    else if (n == 0) return a(m - 1, 1); //if n=0,A(m-1,n)
    else return a(m - 1, a(m, n - 1)); //otherwise , A(m-1,A(m,n-1))
}
int main() {
    int m, n; 
    cout << "請輸入m和n :";
    cin >> m >> n;
    cout << a(m, n) << endl;
    return 0;
}
