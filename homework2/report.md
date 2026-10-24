# 41343121
## 作業二 Polynomial
## 解題說明
本題要實作多項式 (Polynomial) 的相加、相乘與給 x 值的計算，並實作輸入和輸出重載。輸入可能包含重複指數、負係數或未排序的項，程式需要正確處理並輸出可閱讀的格式

例如：輸入項 (3,2), (2,1), (1,0) 應輸出 3x^2 + 2x + 1
### 需求
- 可以接收多項式的輸入（係數與指數，有可能會有未排序或重複指數）
- 計算多項式加法與乘法（需要合併同次項並移除係數為 0 的項）
- 對任意 x 值進行代入求值
- 以我們可閱讀的格式輸出多項式（處理 1 與 -1、整數係數顯示）
### 解題策略
以物件導向為核心這些:

- Term和Polynomial類別
- 多項式加法 Add()
- 多項式乘法 Mult()
- 多項式求值 Eval(x)
## 程式實作
### IDE:
Microsoft Visual Studio Code C/C++

```cpp
#include <iostream>
#include <cmath>
using namespace std;

class Polynomial;

class Term {
    friend class Polynomial;
    friend ostream& operator<<(ostream&, const Polynomial&);
private:
    float coef;
    int exp;
};

class Polynomial {
    friend ostream& operator<<(ostream& out, const Polynomial& Poly);
    friend istream& operator>>(istream& in, Polynomial& Poly);
private:
    Term* termArray;
    int capacity;
    int terms;

public:
    Polynomial() {
        capacity = 10;
        terms = 0;
        termArray = new Term[capacity];
    }

    Polynomial Add(Polynomial Poly) {
        Polynomial Res;
        int a = 0, b = 0;
        while (a < terms && b < Poly.terms) {
            if (termArray[a].exp == Poly.termArray[b].exp) {
                float sum = termArray[a].coef + Poly.termArray[b].coef;
                if (sum != 0)
                    Res.Newterms(sum, termArray[a].exp);
                a++; b++;
            }
            else if (termArray[a].exp > Poly.termArray[b].exp) {
                Res.Newterms(termArray[a].coef, termArray[a].exp);
                a++;
            }
            else {
                Res.Newterms(Poly.termArray[b].coef, Poly.termArray[b].exp);
                b++;
            }
        }
        for (; a < terms; a++)
            Res.Newterms(termArray[a].coef, termArray[a].exp);
        for (; b < Poly.terms; b++)
            Res.Newterms(Poly.termArray[b].coef, Poly.termArray[b].exp);
        return Res;
    }

    Polynomial Mult(Polynomial Poly) {
        Polynomial Res;
        for (int i = 0; i < terms; i++) {
            Polynomial Temp;
            for (int j = 0; j < Poly.terms; j++) {
                float newCoef = termArray[i].coef * Poly.termArray[j].coef;
                int newExp = termArray[i].exp + Poly.termArray[j].exp;
                Temp.Newterms(newCoef, newExp);
            }
            Res = Res.Add(Temp);
        }
        return Res;
    }

    float Eval(float Float) {
        float Res = 0;
        for (int i = 0; i < terms; i++)
            Res += termArray[i].coef * pow(Float, termArray[i].exp);
        return Res;
    }
    void Newterms(float newCoef, int newExp) {
        if (newCoef == 0) return;
        if (terms == capacity) {
            capacity *= 2;
            Term* Temp = new Term[capacity];
            for (int i = 0; i < terms; i++)
                Temp[i] = termArray[i];
            delete[] termArray;
            termArray = Temp;
        }
        termArray[terms].coef = newCoef;
        termArray[terms].exp = newExp;
        terms++;
    }
};

istream& operator>>(istream& In, Polynomial& Poly) {
    int n;
    cout << "輸入次數: ";
    In >> n;

    Poly.terms = 0;

    for (int i = 0; i < n; i++) {
        float Coef;
        int Exp;
        cout << "輸入第" << i + 1 << "項的係數和指數 " << ": ";
        In >> Coef >> Exp;
        Poly.Newterms(Coef, Exp);
    }

    return In;
}
ostream& operator<<(ostream& Out, const Polynomial& Poly) {
    if (Poly.terms == 0) {
        Out << "0";
        return Out;
    }

    for (int i = 0; i < Poly.terms; i++) {
        float Coef = Poly.termArray[i].coef;
        int Exp = Poly.termArray[i].exp;

        if (i > 0) {
            if (Coef >= 0) Out << " + ";
            else { Out << " - "; Coef = -Coef; }
        }
        else if (Coef < 0) {
            Out << "-";
            Coef = -Coef;
        }

        if (Exp == 0)
            Out << Coef;
        else if (Exp == 1)
            Out << Coef << "x";
        else
            Out << Coef << "x^" << Exp;
    }

    return Out;
}

int main() {
    Polynomial A, B;

    cin >> A;
    cin >> B;
    Polynomial sum = A.Add(B);
    Polynomial prod = A.Mult(B);
    float x;
    cout << "輸入x為多少: ";
    cin >> x;
    cout << "\nA(x) = " << A << endl;
    cout << "B(x) = " << B << endl;
    cout << "Sum = " << sum << endl;
    cout << "Product = " << prod << endl;

    cout << "p(" << x << ") = " << A.Eval(x) << endl;

    return 0;
}

```
## 效能分析
1. 時間複雜度：

    Newterms: $O(1)$   當發生重新配置時 $O(t)$（t =terms)

    Add: $O(n + m)$   以兩指標線性尋找兩個已排序的 term 陣列

    Mult: $O(n * m^2)$   當 m ≈ n 時，最壞情況為 $O(n^3)$

    Eval: $O(n)$   不改變多項式本身，僅常數空間

    Newterms（單次）：均攤  $O(1)$

2. 空間複雜度：

    Newterms: $O(1)$   當須擴充陣列時會配置一個新陣列 Term[capacity*2]，此時短暫同時存在新舊陣列，峰值為 $O(t)$ ，如果長期使用的空間是 O(t)（termArray 的容量，t 為目前 capacity）

    Add: $O(n + m)$   程式總共在記憶體中需要 A、B ，所以此函式儲存量為 $O(m + n)$

    Mult: $O(m * n)$   最多會含有 m * n 個不同項，所以此函式儲存量為 $O(m * n)$

    Eval: $O(1)$   每項一個 pow 呼叫，因此整體  $O(n)$

    istream： $O(1)$ 輸入後 Poly 內部儲存 n 項 $O(n)$

    ostream： $O(1)$

## 測試與驗證
### 測試案例

| 測試案例 | 輸入參數   | 預期輸出  | 實際輸出  | 
|----------|--------------|----------|----------|
| 測試一   |      3     |             |         |       
|          |        3, 2    |            |        | 
|          |        2,1     |            |        |
|          |        1, 0    |    3x^2 + 2x + 1       |    3x^2 + 2x + 1      |  
| 測試二  |      2    |            |         |       
|          |       2, 5    |            |        | 
|          |       3, 1    |       2x^5 + 3x    |   2x^5 + 3x     |
|   $A(x)$  +    $B(x)$     |      |  2x^5 + 3x^2 + 5x + 1       | 2x^5 + 3x^2 + 5x + 1        |   
|   $A(x)$ * $B(x)$  |     |  6x^7 + 4x^6 + 2x^5 + 9x^3 + 6x^2 + 3x       |   6x^7 + 4x^6 + 2x^5 + 9x^3 + 6x^2 + 3x              |
|   $A(2)$      |      | 17    |   17  |

### 測試輸入
```
輸入次數: 3
輸入第1項的係數和指數 : 3 2
輸入第2項的係數和指數 : 2 1
輸入第3項的係數和指數 : 1 0
輸入次數: 2
輸入第1項的係數和指數 : 2 5
輸入第2項的係數和指數 : 3 1
輸入x為多少: 2
```
### 測試輸出
```
A(x) = 3x^2 + 2x + 1
B(x) = 2x^5 + 3x
Sum = 2x^5 + 3x^2 + 5x + 1
Product = 6x^7 + 4x^6 + 2x^5 + 9x^3 + 6x^2 + 3x
p(2) = 17
```


## 申論及開發報告
### 申論
#### Polynomial 類別
屬性包含：

- termArray：動態陣列儲存多項式項
- capacity：陣列容量
- terms：實際項數

實作方法：

- Add()：多項式加法
- Mult()：多項式乘法
- Eval()：代入求值
- Newterms()：新增項目（並自動擴充記憶體）
#### 多項式加法 Add()
- 依據指數從大到小比對
- 若指數相同，則係數相加
- 若不同，則將較大指數的項直接加入結果多項式中
#### 多項式乘法 Mult()
- 採用雙層迴圈將每一項彼此相乘，並利用加法函式整合重複指數的項：
  ```cpp
  
  for (int i = 0; i < terms; i++) {
    for (int j = 0; j < Poly.terms; j++) {
        newCoef = coef_i * coef_j;
        newExp = exp_i + exp_j;
        Temp.Newterms(newCoef, newExp);
    }
    Res = Res.Add(Temp);
  }
  
#### 多項式求值 Eval(x)
- 將輸入的 x 值代入多項式：

$$P(x) = \sum_i (coef_i \times x^{exp_i})$$


對每一項計算後累加得結果。

### 結論

本次作業充分的練習到了物件導向,對多項式運算的邏輯化設計有更深理解。
此多項式系統成功實現了基礎的多項式四則運算，展示了 C++ 類別設計與動態陣列管理的應用。
程式結構清晰，擴充性高，適合作為教學與進階功能開發的基礎。
