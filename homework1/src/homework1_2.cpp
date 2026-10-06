#include <iostream>
#include <string>

using namespace std;

void push(string s[], int& top, const string& v) {
    s[top++] = v;
}
void pop(int& top) {
    if (top > 0) {
        top--;
    }
}
void powerset(string A[], string s[], int& top, int i, int n, bool& r) {
   
    if (i == n) { // 終止條件：已考慮完全部元素
        if (!r) {
            cout << ", ";
        }
        r = false;

        cout << "(";
        for (int j = 0; j < top; j++) {//印出目前元素,當子集是空集合（top == 0）時，迴圈一次都不會執行
            cout << s[j];
            if (j != top - 1) cout << ", ";
        }
        cout << ")";
        return;
    }
    powerset(A, s, top, i + 1, n, r);// 不選取A[i]分支，繼續遞迴往下探索，直到最後碰到底部（i == n）時才會印出該完整組合
    push(s, top, A[i]);// 選取當前元素 A[i]，使用 push 加入放進s[]
    powerset(A, s, top, i + 1, n, r);
    pop(top); // 使用 pop 將 A[i] 移除，復原狀態s[]變空空
}

int main() {
    int n;
    cout << "請輸入集合大小 n : ";
    cin >> n;

    string* A = new string[n];
    string* s = new string[n];

    int top = 0; //指標存一下

    cout << "請輸入集合元素 : ";
    for (int i = 0; i < n; i++) {
        cin >> A[i];
    }

    cout << "S = {";
    
    bool r = true;

    powerset(A, s, top, 0, n, r);
    
    cout << "}" << endl;

    delete[] A;
    delete[] s;

    return 0;
}