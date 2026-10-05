# 41443105

## 作業一 之Ackermann Function

## 解題說明

本題要求用遞迴函式和非遞迴函式，計算 Ackermann 函數 $A(m,n)$ 的值。Ackermann 函數是一個以兩個正整數 m 和 n 為輸入的特殊遞迴函數，其結果會隨著 m, n 的增大而迅速變大。

### Ackermann 函數定義如下 :
| A(m,n)                 | 條件                  |
|------------------------|----------------------|
| n + 1                  | 當 m = 0             |
| A(m-1, 1)              | 當 n = 0             |
| A(m-1, A(m, n-1))      | 其他的所有可能        |

### 解題策略

1. 遞迴函式:
   按照題目的條件下去寫，使程式一層一層的叫自己的方式直到結束條件，而結束的條件是所有遞迴測資 m == 0的時候。
2. 非遞迴函式:
   我設定一個足夠大的 stack 來模擬遞迴while重複執行加上if判斷所有測資的 m 是否等於 0，其中 MAX_STACK = 100000; 依需求調整實際大小。

## 程式實作

### IDE:
Microsoft Visual Studio Code C/C++

遞迴程式：

```cpp
#include <iostream>
using namespace std;
int ack(int m, int n) {
    if (m == 0) return n + 1; //if m=0,n+1
    else if (n == 0) return ack(m - 1, 1); //if n=0,A(m-1,n)
    else return ack(m - 1, ack(m, n - 1)); //otherwise , A(m-1,A(m,n-1))
}
int main() {
    int m, n;
    cout << "輸入 m 和 n : ";
    while (cin >> m >> n) {
        cout << "(" << m << ", " << n << ") = " << ack(m, n) << endl;
        cout << "輸入 m 和 n : ";
    }
    return 0;
}
```
非遞迴程式：

```cpp
#include <iostream>
#include <cstring> // for memset
using namespace std;

int ackermann(int m, int n) {
    // 設定一個足夠大的 stack 來模擬遞迴
    const int MAX_STACK = 100000; // 可依需求調整
    int stack[MAX_STACK];
    int top = 0;
    stack[top++] = m;

    while (top > 0) {
        m = stack[--top];
        if (m == 0) {
            if (top == 0) {
                return n + 1;
            }
            n = n + 1;
        }
        else if (n == 0) {
            stack[top++] = m - 1;
            n = 1;
        }
        else {
            stack[top++] = m - 1;
            stack[top++] = m;
            n = n - 1;
        }
    }
    return n;
}

int main() {
    int m, n;
    cout << "輸入 m 和 n :  ";
    while (cin >> m >> n) {
        cout << "(" << m << ", " << n << ") = " << ack(m, n) << endl;
        cout << "輸入 m 和 n : ";
    }
    return 0;
}
```

## 效能分析
遞迴函式：
1. 空間複雜度：當 m, n 較小時（如 m=3, n=5），遞迴層數可控；當 m, n 較大時，遞迴層數會極速膨脹，很快超過程式語言的棧上限，導致 Stack Overflow

   m=0、1、2 時，空間複雜度分別為 $O(1)$ 、 $O(n)$ 、 $O(n)$ 。m=3 時，空間複雜度約 $O(2ⁿ)$ 。m≥4 時，空間複雜度超越 $O(2ⁿ)$
   
2. 時間複雜度：對於小 m（0, 1, 2, 3），可以用簡單公式計算，複雜度為 $O(1)$ 、 $O(n)$ 或 $O(2ⁿ)$ ，但 m=4 以後，複雜度超過指數級，遠大於 $O(2ⁿ)$ 。

非遞迴函式：
1. 空間複雜度：m=3 時，空間複雜度 $O(n)$ ；m=4 時，空間複雜度 $O(2^n)$ ；m≥5 時，空間複雜度超越 O(2^(2^(...2^n)))（n層）
2. 時間複雜度：對於 m=3，複雜度約 $O(2^n)$ ；m=4，複雜度約 O(2^(2^(...2^n))) （n 層）。實際上，計算 Ackermann(4, 2) 就需要幾億次操作。

## 測試與驗證

### 測試案例 (遞迴/非遞迴)

| 測試案例 | 輸入參數 $m,n$ | 預期輸出 | 實際輸出 |
|----------|--------------|----------|----------|
| 測試一   | $m = 1,n = 1$ | 3        | 3        |
| 測試二   | $m = 2,n = 1$ | 5        | 5        |
| 測試三   | $m = 3,n = 2$ | 29        | 29        |
| 測試四   | $m = 3,n = 4$ | 125       | 125       |
| 測試五   | $m = 5,n = 1$ | 異常輸出  | 異常輸出  |

### 編譯與執行指令

```shell
$ g++ ack.cpp -std=c++17 -o ack.exe
$ .\ack.exe
輸入 m 和 n : 2 0
(2, 0) = 3
輸入 m 和 n : 2 1
(2, 1) = 5
```
### 結論

1. 在開發過程中使用了堆疊來記錄每個側資的關係式但還是應為空間不足，大概輸入(4,2)以上時就無法執行了。
2. 不管是遞迴或是非遞迴的分析都會發現牠們的時間複雜度相當大尤其是非遞迴，設計預期是想解決時間問題但由於(Ackermann Function)會隨著 m, n 的增大而迅速變大，
   最後還是無法解決時間問題。  
3. 測試中涵蓋了許多的變因情況，例如異常輸出等驗證不通過但如果使用指數關係式也許會比較好解決吧。

## 申論及開發報告

### 選擇遞迴方法的原因

1. **程式邏輯直觀**

   遞迴能清楚表達 Ackermann 函數 的核心概念：

   當 𝑚=0時，可以直接回傳 𝑛+1 。

   當 m > 0 且 n = 0，回傳 Ackermann(m-1, 1)。

   當 m > 0 且 n > 0，回傳 Ackermann(m-1, Ackermann(m, n-1))。

   這三種情況能用 if/else 結構清楚實現，邏輯分明。

3. **遞迴清楚**

   每次遞迴都根據 m 與 n 的值進行分支，呼叫自身，且每個分支都明確，沒有複雜的前置處理或後續工作。這樣遞迴結構便於追蹤、除錯。
4. **優缺點**

   優點:直觀反映函數本質、程式碼簡潔、易懂

   缺點:容易發生 Stack Overflow、效能低下、實用性不高
--------------------------------------------------------------------------------------------------------------------------------------------------------------------

## 作業一 之Powerset

## 解題說明

題目要求使用遞迴函式的方法來輸出集合S()的所有子集合。

例如: 元素設3個空間 , S =(G,C,D) , 請問 powerset(S) 會等於多少呢?
輸出: (),(G),(C),(D), (G,C), (G,D), (D,G), (G,C,D) } 

### 解題策略
這題原本要用二維陣列，但二維陣列會浪費大量記憶體空間，因此改用兩個動態記憶體配置來取代，並能改善使用者的測資輸入問題。輸出部分可以透過 for 迴圈與記憶體位置來判斷，而使用者的測資可能會有重複問題，因此可以加入 if 判斷式來提升程式的準確性。

## 程式實作

### IDE:
Microsoft Visual Studio Code C/C++

```cpp
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
```

## 效能分析
1. 空間複雜度：總空間複雜度 $O(n)$
2. 時間複雜度：總時間複雜度 $O(n * 2^n)$

## 測試與驗證

### 測試案例

| 測資 | 輸入參數 $n$ , 元素 | 預期輸出 | 實際輸出 |
|----------|--------------|----------|----------|
| 測試一   | $n = 1$ , a | () (a)       | () (a)       |
| 測試二   | $m = 2$ , a b | () (b) (a) (a,b)  | () (b) (a) (a,b)       |
| 測試三   | $m = 3$ , a b c | () (c) (b) (b,c) (a) (a,c) (a,b) (a,b,c) | () (c) (b) (b,c) (a) (a,c) (a,b) (a,b,c) |
### 編譯與執行指令

```shell
$ g++ pow.cpp -std=c++17 -o pow.exe
$ .\pow.exe
請輸入n個數 : 3
請輸入集合元素 : a b c
集合的冪集為:
{}
{c}
{b}
{b,c}
{a}
{a,c}
{a,b}
{a,b,c}
```

### 結論

1. 每多一個元素，子集數量就會變成兩倍（也就是 2ⁿ 的成長），所以當 n 很大時，程式會變得很慢、也很吃記憶體。
2. 可以加入 if 判斷式來提升程式的準確性，當使用者的測資有重複時，可以排除有一樣的子集合

## 申論及開發報告

### 選擇遞迴式回溯法的原因

1. 遞迴直觀：

   程式邏輯清楚，對每個元素只需考慮是否加入子集合的兩種情況。

2. 動態陣列：
   
   利用 new 配置陣列大小，執行時可依使用者輸入決定集合大小，不會浪費大量記憶體空間。
   
3. **優缺點**

   優點：程式結構簡潔、易於理解、不需額外資料結構

   缺點：效能瓶頸明顯、時間複雜度高
