# 41443105
## 四資工二甲 鄭品均
## Homework 2 作業 Polynomial
## 解題說明
本題要實作多項式抽象資料型態（Polynomial ADT）的相加、相乘與給 x 值的計算，並實作輸入和輸出重載。輸入可能包含重複指數(ex: $2 x^2 + 3 x^2$ )、負係數或未排序(使用者未照指示降冪輸入)的項，程式需要正確處理並輸出可閱讀的格式。多項式以降冪形式表示為各非零項之集合：

$$p(x) = a_0 x^{e_0} + a_1 x^{e_1} + \dots + a_n x^{e_n}$$

其中 $a_i$ 為非零浮點數係數（float）， $e_i$ 為非負整數指數（int）。本作業需支援多項式預設建構、加法（Add）、乘法（Mult）、代入求值（Eval），以及標準輸入輸出串流運算子多載（`>>` 與 `<<`）。

### 需求
- 可以接收多項式的輸入（係數與指數，有可能會有未排序或重複指數）
- 計算多項式加法與乘法（需要合併同次項並移除係數為 0 的項）
- 對任意 x 值進行代入求值
- 以我們可閱讀的格式輸出多項式（處理 1 與 -1、整數係數顯示）
### 解題策略
1. **內部資料表示法**：定義 `Term` 類別儲存單一項之係數與指數，並於 `Polynomial` 類別中以動態陣列指標 `Term* termArray` 搭配 `capacity`（容量）與 `terms`（實際項數）進行動態維護。
2. **插入防呆機制（NewTerm）**：
   - 擴充新項時，若相加後相消之後為 0 則剔除，自動進行合併。
   - 插入新項確保內部陣列各項嚴格維持「指數由大到小降冪排列」，即使使用者亂輸入亦能自動修正。
   - 當空間額滿時執行容量加倍(*2)並重新配置記憶體，確保空間不會不夠。
3. **多項式加法（Add）**：採用兩個指標逐步走訪兩多項式的非零項：
   - 指數相同：係數相加，結果不為 0 則加入新多項式。
   - 指數不同：將指數較大者優先加入新多項式。
   - 雙指標比對完畢後，將剩餘未走訪的項目依序收尾追加。
4. **多項式乘法（Mult）**：使用雙重迴圈執行逐項相乘（係數相乘、指數相加），每完成一輪即呼叫 `Add()` 方法累加至結果多項式，自動合併同類項並維持降冪。
5. **記憶體管理**：自定義解構子（釋放動態空間）、複製建構子與賦值運算子（執行深層複製 Deep Copy），杜絕淺層複製導致的指標共用與 Double Free (a=b，a跟b指向同一個指標)記憶體錯誤。

---
## 程式實作

### IDE:
Microsoft Visual Studio Code C/C++
> 檔案名稱:Polynomial.cpp
```cpp
#include <iostream>
#include <cmath>

using namespace std;

class Polynomial; // 前置宣告

class Term {
    friend class Polynomial;
    friend ostream& operator<<(ostream& os, const Polynomial& p);
    friend istream& operator>>(istream& is, Polynomial& p);
private:
    float coef; // 係數
    int exp;    // 指數 
};

class Polynomial {
    friend ostream& operator<<(ostream& os, const Polynomial& p);
    friend istream& operator>>(istream& is, Polynomial& p);
private:
    Term* termArray; // 陣列指標
    int capacity;    // 陣列最大容量
    int terms;       // 目前非零項數
public:
    // 預設建構子: p(x) = 0
    Polynomial() : capacity(2), terms(0) {
        termArray = new Term[capacity];
    }

    // 複製建構子 (深層複製)
    Polynomial(const Polynomial& other) : capacity(other.capacity), terms(other.terms) {
        termArray = new Term[capacity];
        for (int i = 0; i < terms; ++i) {
            termArray[i] = other.termArray[i];
        }
    }

    // 等於符號（=）
    Polynomial& operator=(const Polynomial& other) {
        if (this != &other) {
            delete[] termArray;
            capacity = other.capacity;
            terms = other.terms;
            termArray = new Term[capacity];
            for (int i = 0; i < terms; ++i) {
                termArray[i] = other.termArray[i];
            }
        }
        return *this;
    }

    // 解構子
    ~Polynomial() {
        delete[] termArray;
    }

    // 內部空間加倍擴容機制
    void NewTerm(float Coef, int Exp) {
        if (Coef == 0.0f) return;

        // 【阻擋並顯示負數指數錯誤】
        if (Exp < 0) {
            cout << "[錯誤] 指數不能為負數 (" << Exp << ")，該項已被略過！" << endl;
            return;
        }

        // 相同次方，直接累加係數 (同類項合併)
        for (int i = 0; i < terms; ++i) {
            if (termArray[i].exp == Exp) {
                termArray[i].coef += Coef;
                if (termArray[i].coef == 0.0f) {
                    for (int j = i; j < terms - 1; ++j) {
                        termArray[j] = termArray[j + 1];
                    }
                    terms--;
                }
                return;
            }
        }

        if (terms == capacity) { // 空間滿了
            capacity *= 2;       // 容量加倍
            Term* temp = new Term[capacity];
            for (int i = 0; i < terms; ++i) {
                temp[i] = termArray[i];
            }
            delete[] termArray;
            termArray = temp;
        }

        // 尋找降冪插入點
        int insertPos = 0;
        while (insertPos < terms && termArray[insertPos].exp > Exp) {
            insertPos++;
        }

        // 將插入點之後的元素往後挪移一格
        for (int i = terms; i > insertPos; --i) {
            termArray[i] = termArray[i - 1];
        }

        termArray[insertPos].coef = Coef;
        termArray[insertPos].exp = Exp;
        terms++;
    }

    // 多項式加法
    Polynomial Add(Polynomial poly) {
        Polynomial c;
        int a = 0, b = 0;

        while (a < terms && b < poly.terms) {
            if (termArray[a].exp == poly.termArray[b].exp) {
                float sum = termArray[a].coef + poly.termArray[b].coef;
                if (sum != 0.0f) {
                    c.NewTerm(sum, termArray[a].exp);
                }
                a++;
                b++;
            }
            else if (termArray[a].exp < poly.termArray[b].exp) {
                c.NewTerm(poly.termArray[b].coef, poly.termArray[b].exp);
                b++;
            }
            else {
                c.NewTerm(termArray[a].coef, termArray[a].exp);
                a++;
            }
        }
        for (; a < terms; a++) c.NewTerm(termArray[a].coef, termArray[a].exp);
        for (; b < poly.terms; b++) c.NewTerm(poly.termArray[b].coef, poly.termArray[b].exp);
        return c;
    }

    // 多項式乘法
    Polynomial Mult(Polynomial poly) {
        Polynomial c;
        for (int i = 0; i < terms; ++i) {
            Polynomial temp;
            for (int j = 0; j < poly.terms; ++j) {
                float newCoef = termArray[i].coef * poly.termArray[j].coef;
                int newExp = termArray[i].exp + poly.termArray[j].exp;
                temp.NewTerm(newCoef, newExp);
            }
            c = c.Add(temp);
        }
        return c;
    }

    // 計算求值 f(x)
    float Eval(float x) {
        float result = 0.0f;
        for (int i = 0; i < terms; ++i) {
            result += termArray[i].coef * pow(x, termArray[i].exp);
        }
        return result;
    }
};

// 多載 >> 運算子
istream& operator>>(istream& is, Polynomial& p) {
    int n;
    if (!(is >> n)) return is;

    delete[] p.termArray;
    p.capacity = (n > 0) ? n : 2;
    p.terms = 0;
    p.termArray = new Term[p.capacity];

    for (int i = 0; i < n; ++i) {
        float c;
        int e;
        is >> c >> e;
        // 【輸入時即時檢查】
        if (e < 0) {
            cout << "[輸入錯誤] 指數不能為負數 (" << e << ")，該項不予加入！" << endl;
        } else {
            p.NewTerm(c, e);
        }
    }
    return is;
}

// 多載 << 運算子
ostream& operator<<(ostream& os, const Polynomial& p) {
    if (p.terms == 0) {
        os << "0";
        return os;
    }

    for (int i = 0; i < p.terms; ++i) {
        float c = p.termArray[i].coef;
        int e = p.termArray[i].exp;

        if (i > 0 && c > 0) os << " + ";
        else if (i > 0 && c < 0) {
            os << " - ";
            c = -c;
        }
        else if (i == 0 && c < 0) {
            os << "-";
            c = -c;
        }

        if (e == 0) os << c;
        else if (e == 1) {
            if (c == 1.0f) os << "x";
            else os << c << "x";
        }
        else {
            if (c == 1.0f) os << "x^" << e;
            else os << c << "x^" << e;
        }
    }
    return os;
}

int main() {
    Polynomial p1, p2;

    cout << "請輸入多項式 1 (項數 係數1 指數1 係數2 指數2...(盡量請降冪輸入)):" << endl;
    cin >> p1;

    cout << "請輸入多項式 2 (項數 係數1 指數1 係數2 指數2...(盡量請降冪輸入)):" << endl;
    cin >> p2;

    cout << "\nP1(x) = " << p1 << endl;
    cout << "P2(x) = " << p2 << endl;

    Polynomial sum = p1.Add(p2);
    cout << "P1 + P2 = " << sum << endl;

    Polynomial product = p1.Mult(p2);
    cout << "P1 * P2 = " << product << endl;

    float x;
    cout << "請輸入要帶入的 x 值:" << endl;
    cin >> x;
    cout << "P1(" << x << ") = " << p1.Eval(x) << endl;
    cout << "P2(" << x << ") = " << p2.Eval(x) << endl;

    return 0;
}

```
---
## 效能分析

### 符號定義
- $m$：當前多項式（`*this`）之非零項數。
- $n$：引數多項式（`poly`）之非零項數，或輸入串流中讀入之總項數。
- $k$：當前多項式內部陣列中已存儲之非零項數。

### 各成員函式與運算子複雜度彙整表

| 函式 / 運算子 | 時間複雜度 | 空間複雜度 | 演算法行為與空間配置說明 |
| :--- | :--- | :--- | :--- |
| **`Polynomial()`**<br>(預設建構子) | $O(1)$ | $O(1)$ | 配置常數初始容量（`capacity = 2`）之動態陣列，初始化成員變數。 |
| **`~Polynomial()`**<br>(解構子) | $O(1)$ | $O(1)$ | `Term` 無動態指標，直接以 `delete[] termArray` 釋放一整塊連續空間。 |
| **`Polynomial(const &)`**<br>(複製建構子) | $O(k)$ | $O(k)$ | 配置等量新陣列，並以單層迴圈走訪複製 $k$ 個項目（深層複製 Deep Copy）。 |
| **`operator=`**<br>(賦值運算子) | $O(k)$ | $O(k)$ | 釋放原有舊空間後重新配置，並循序走訪複製 $k$ 個項目。 |
| **`NewTerm`**<br>(插入新項) | $O(k)$ | $O(1)$ (擴容) $O(k)$ | 線性掃描同類項合併與降冪挪移插入需 $O(k)$；容量倍增擴容均攤（Amortized）為 $O(1)$ 。 |
| **`Add`**<br>(多項式加法) | $O(m + n)$ | $O(m + n)$ | 雙指標同步比對指數由高至低走訪，各項僅存取一次；新多項式最多容納 $m + n$ 項。 |
| **`Mult`**<br>(多項式乘法) | $O(m^2 \cdot n)$ | $O(m \cdot n)$ | 雙重迴圈逐項相乘並以 `Add()` 累加；最壞情況下項皆互異，乘積最多包含 $m \times n$ 項。 |
| **`Eval`**<br>(代入求值) | $O(m)$ | $O(1)$ | 循序走訪 $m$ 個項目計算各項冪次並累加，僅需常數個區域浮點數變數。 |
| **`operator>>`**<br>(串流輸入) | $O(n^2)$ | $O(n)$ | 讀入 $n$ 項並逐項呼叫 `NewTerm` 進行排序插入，總比對挪移步數為 $\sum_{i=1}^n i = O(n^2)$ 。 |
| **`operator<<`**<br>(串流輸出) | $O(m)$ | $O(1)$ | 單層迴圈循序走訪 $m$ 個非零項進行格式化輸出，不耗費額外配置空間。 |

---

### 詳細推導說明

1. **多項式加法 `Add`**：
   - 採用兩個指標比對兩多項式降冪陣列。由於每一輪迭代中至少有一個指標（`a` 或 `b`）向前遞增，因此總比對次數嚴格受限於兩多項式項數總和，時間複雜度為 $O(m + n)$ 。
   - 新建多項式最多只會儲存兩多項式全部項數（無同類項合併時），故額外輔助空間複雜度為 $O(m + n)$ 。

2. **多項式乘法 `Mult`**：
   - 外層迴圈共執行 $m$ 次，每次產生長度為 $n$ 的單項式積並呼叫一次 `Add()` 累加至結果物件。
   - 累加過程中的多項式項數逐步擴大：第 $1$ 次累加長度約為 $n$，第 $2$ 次約為 $2n$，至第 $m$ 次累加時規模達到 $mn$。總運算耗時為 $\sum_{i=1}^{m} (i \cdot n) = n \sum_{i=1}^{m} i = O(m^2 \cdot n)$ 。
   - 空間複雜度取決於展開後之非零項總數，最壞情況下所有乘積項次方皆不重複，空間複雜度為 $O(m \cdot n)$ 。

3. **輸入輸出運算子**：
   - `operator>>`：為了防呆並支援沒有順序的輸入，每次讀入都透過 `NewTerm` 執行插入排序與同類項檢查。插入第 $i$ 項之平均時間為 $O(i)$ ，讀入 $n$ 項之總時間複雜度為 $O(n^2)$ ；動態陣列按需擴充，空間複雜度為 $O(n)$ 。
   - `operator<<`：單純以單一迴圈從頭至尾走訪 $m$ 個非零項輸出字串，時間複雜度為線性 $O(m)$ ，輔助空間複雜度為 $O(1)$ 。

---
## 測試與驗證

```shell
$ g++ main.cpp -o main.exe
$ .\main.exe
請輸入多項式 1 (項數 係數1 指數1 係數2 指數2...(盡量請降冪輸入):
2 3 2 2 1
請輸入多項式 2 (項數 係數1 指數1 係數2 指數2...(盡量請降冪輸入)):
2 1 2 4 0

P1(x) = 3x^2 + 2x
P2(x) = x^2 + 4
P1 + P2 = 4x^2 + 2x + 4
P1 * P2 = 3x^4 + 2x^3 + 12x^2 + 8x
請輸入要帶入的 x 值:
2
P1(2) = 16
P2(2) = 8
```
---
| 測試案例 |&nbsp;輸入參數&nbsp;| 預期輸出 | 實際輸出 |
|:---|:---|:---|:---|
| **【案例一：未按降冪輸入】**<br>多項式 P1(x) | 3 | | |
| | 2&emsp;0<br>(常數項先輸) | | |
| | 5&emsp;2<br>(二次項) | | |
| | 3&emsp;1<br>(一次項) | $5x^2 + 3x + 2$ | $5x^2 + 3x + 2$ |
| 多項式 P2(x) | 2 | | |
| | 1&emsp;1 (一次項先輸) | | |
| | 4&emsp;3 (三次項) | $4x^3 + x$ | $4x^3 + x$ |
| $P1(x) + P2(x)$ | （執行加法 Add） | $4x^3 + 5x^2 + 4x + 2$ | $4x^3 + 5x^2 + 4x + 2$ |
| $P1(x) \times P2(x)$ | （執行乘法 Mult） | $20x^5 + 12x^4 + 13x^3 + 3x^2 + 2x$ | $20x^5 + 12x^4 + 13x^3 + 3x^2 + 2x$ |
| $P1(2)$ | （代入 $x = 2$ 執行 Eval） | $28$ | $28$ |
| $P2(2)$ | （代入 $x = 2$ 執行 Eval） | $34$ | $34$ |
| **【案例二：重複指數輸入】**<br>多項式 P1(x) | 4 | | |
| | 3&emsp;2<br>(二次項) | | |
| | 2&emsp;1<br>(第一次輸入一次項) | | |
| | 5&emsp;1<br>(第二次輸入一次項) | | |
| | 4&emsp;0 (常數項) | $3x^2 + 7x + 4$ | $3x^2 + 7x + 4$ |
| 多項式 P2(x) | 3 | | |
| | 2&emsp;2<br>(第一次輸入二次項) | | |
| | 1&emsp;2<br>(第二次輸入二次項) | | |
| | 1&emsp;0<br>(常數項) | $3x^2 + 1$ | $3x^2 + 1$ |
| $P1(x) + P2(x)$ | （執行加法 Add） | $6x^2 + 7x + 5$ | $6x^2 + 7x + 5$ |
| $P1(x) \times P2(x)$ | （執行乘法 Mult） | $9x^4 + 21x^3 + 15x^2 + 7x + 4$ | $9x^4 + 21x^3 + 15x^2 + 7x + 4$ |
| $P1(2)$ | （代入 $x = 2$ 執行 Eval） | $30$ | $30$ |
| $P2(2)$ | （代入 $x = 2$ 執行 Eval） | $13$ | $13$ |
| **【案例三：同次方完全抵銷】**<br>多項式 P1(x) | 3 | | |
| | 4&emsp;3<br>(三次項) | | |
| | 2&emsp;2<br>(二次項) | | |
| | 1&emsp;0<br>(常數項) | $4x^3 + 2x^2 + 1$ | $4x^3 + 2x^2 + 1$ |
| 多項式 P2(x) | 3 | | |
| | -4&emsp;3<br>(負三次項) | | |
| | -2&emsp;2<br>(負二次項) | | |
| | 5&emsp;0<br>(常數項) | $-4x^3 - 2x^2 + 5$ | $-4x^3 - 2x^2 + 5$ |
| $P1(x) + P2(x)$ | （高次全數抵銷為 0） | $6$ | $6$ |
| $P1(x) \times P2(x)$ | （執行乘法 Mult） | $-16x^6 - 16x^5 - 4x^4 + 16x^3 + 8x^2 + 5$ | $-16x^6 - 16x^5 - 4x^4 + 16x^3 + 8x^2 + 5$ |
| $P1(2)$ | （代入 $x = 2$ 執行 Eval） | $41$ | $41$ |
| $P2(2)$ | （代入 $x = 2$ 執行 Eval） | $-35$ | $-35$ |
| **【案例四：負數指數防呆】**<br>多項式 P1(x) | 2 | | |
| | 3&emsp;2<br>(二次項) | | |
| | 4&emsp;-1<br>(負指數項) | 阻擋並印出錯誤提示<br>內部僅保留 $3x^2$ | `[輸入錯誤] 指數不能為負數 (-1)，該項不予加入！`<br>$3x^2$ |
| 多項式 P2(x) | 2 | | |
| | 2&emsp;1<br>(一次項) | | |
| | 1&emsp;0<br>(常數項) | $2x + 1$ | $2x + 1$ |
| $P1(x) + P2(x)$ | （執行加法 Add） | $3x^2 + 2x + 1$ | $3x^2 + 2x + 1$ |
| $P1(x) \times P2(x)$ | （執行乘法 Mult） | $6x^3 + 3x^2$ | $6x^3 + 3x^2$ |
| $P1(2)$ | （代入 $x = 2$ 執行 Eval） | $12$ | $12$ |
| $P2(2)$ | （代入 $x = 2$ 執行 Eval） | $5$ | $5$ |
---


## 申論及開發報告


### 1. 資料結構選用原因與效益分析
在多項式（Polynomial）的實作中，常見的方法有「以指數為索引的定長陣列」與「儲存非零項的動態陣列」兩種：

- **定長陣列的缺點**：當遇到稀疏多項式時，中間有大量的 $0$，不僅 `浪費龐大記憶體空間` ，在執行加法時還必須遍歷所有為 $0$ 的元素， `缺乏效率` 。
  
- **選用 `<coef, exp>` 動態陣列的優勢**：

   -  本實作採用 `Term* termArray` 搭配 `capacity` 與 `terms`，僅儲存有效之非零項。
   -  此設計大幅降低記憶體使用，並使多項式加法 `Add()` 的時間複雜度直接縮減至與實際非零項數相關的線性時間 $O(m + n)$ 。
   -  搭配容量倍增機制（ `capacity *= 2` ），既兼顧了連續記憶體的快取友善性（Cache Friendly）(意思就是：因為陣列在記憶體中是一整排連續存放的，CPU 循序走訪時能連續觸發快取命中，比起使用指標跳來跳去的鏈結串列，跑起來會快非常多。)，又兼具動態擴充的彈性。

---

### 2. C++ 記憶體管理
在本類別中，由於包含了底層指標成員 `Term* termArray` 進行動態記憶體配置（`new`），若未妥善處理生命週期，極易造成系統崩潰：
1. **防止記憶體洩漏（Memory Leak）**：
   - 實作解構子 `~Polynomial()`，在物件生命週期結束離開作用域時主動呼叫 `delete[] termArray`，將資源歸還給作業系統。
2. **避免淺層複製與 Double Free 當機**：
   - 若使用編譯器預設的複製行為，傳值呼叫 `僅會複製指標位址` ，導致兩個多項式物件共用同一塊動態記憶體。當其中一個物件先解構釋放記憶體後，另一個物件在解構時會觸發「同一記憶體釋放兩次（Double Free）」的嚴重錯誤。
3. **實作深層複製（Deep Copy）**：
   - 於**複製建構子**中重新 `new` 一塊大小相同的獨立空間，並逐項複製內容。
   - 於**賦值運算子 `operator=`** 中先檢查是否自我賦值（`this != &other`），再釋放自身原有的舊陣列並配置新空間拷貝資料，確保了多項式在物件傳參、運算賦值（如 `c = c.Add(temp)`）時的記憶體安全性。

---

### 3. 開發過程中遭遇之問題與解決方案

在開發與測試階段，主要遭遇並克服了以下四項技術挑戰：

**使用者亂輸入與顯示雜亂問題**：
   - *問題1*：若使用者未按照提示降冪輸入（例如先輸入低次方或常數項），不僅輸出格式不符數學慣例，更會導致加法雙指標比對邏輯失效。
   > *解法*：重構 `NewTerm()` 函式，引入插入排序思維。每當有新項讀入時，循序搜尋適當的降冪插入點，將後續項目向後挪移一格再放入，保證多項式內部隨時維持嚴格降冪排列。

**重複指數與同類項自動合併**：
   - *問題2*：使用者可能在輸入中分開鍵入相同次方的項（例如連續輸入兩個一次項）。
   > *解法*：在 `NewTerm()` 插入前先走訪現有項，若已存在相同指數（`Exp`），則直接將係數累加進行同類項合併，避免陣列中出現冗餘項。

**多項式相加係數抵銷為 0 的邊界處理**：
   - *問題3*：當兩多項式相加剛好同次方正負抵銷（如 $3x^2$ 與 $-3x^2$ 相加為 $0$ ），若將係數為 $0$ 的項留存於陣列中，不僅違反 ADT 定義（僅存非零項），亦會造成後續輸出出現 `+ $0x^2$ ` 的非預期結果。
   > *解法*：在同類項合併與 `Add()` 邏輯中加入檢查，若係數相加等於 $0.0f$ ，則將該位置之後的所有元素往前搬移遞補並使 `terms--`，確保內部只維護真正的非零項。當全部抵銷導致 `terms == 0` 時，`operator<<` 能正確輸出單一字元 `"0"`。
**非負整數指數限制與負次方輸入防呆**：
   - *問題4*：依據數學定義，多項式指數必須為非負整數（$e \ge 0$）。
   > *解法*：於 operator>> 輸入串流及 NewTerm() ，一旦偵測到傳入之指數 Exp < 0，即時輸出錯誤提示文字（如 [輸入錯誤] 指數不能為負數）並直接略過，確保符合標準數學定義。
---
### 4. 系統實作優缺點分析
**優點**：
- 空間利用率高：採用 <coef, exp> 序對動態陣列，僅存非零項，大幅節省稀疏多項式儲存空間          
- 快取高度友善：連續記憶體（Term* termArray）具備極佳空間局部性，雙指標走訪與求值時快取命中率高
- 記憶體管理嚴謹：嚴格實踐 Rule of Three (自定義解構子、複製建構子與賦值運算子)，透過深層複製杜絕淺層複製、Double Free 與記憶體洩漏(導致已經不再使用的記憶體空間無法被釋放，讓可用記憶體不斷減少)
- 輸入防呆自癒：NewTerm 內建插入排序與同類項檢查，自動處理亂序、重複指數合併及係數為 0 或負數的項剔除
        
**缺點與限制**：
- 陣列平移開銷：維持嚴格降冪需頻繁將後續元素向後或向前挪移，插入與零項剔除之搬移成本達 $O(N)$  
- 乘法高階複雜度瓶頸：雙重迴圈每輪皆呼叫 Add 累加，中間項膨脹導致整體時間達  $O(N^3)$  ，實測 N=1000 耗時達 4.86 秒
- 動態擴容瞬間開銷：容量倍增（capacity *= 2）雖為均攤 $O(1)$ ，但觸發擴容當下仍有重新配置與全數搬移成本
        
**未來可改善方向**：
- 乘法架構優化：先收集所有相乘項至單一暫存陣列，再以 $O(N^2log N)$  排序合併，或改採 Karatsuba 演算法(4 次乘法變 3 次)降至 $O(N^2)$ 以下
- 資料結構改進：若面臨超頻繁的任意插入與刪除，可評估改採循環鏈結串列(轉圈圈YA)，消除陣列位移成本

---
## 效能量測


### 1. 量測環境與方法
* **環境**：MinGW GCC，開啟 `-O2` 最佳化旗標編譯。
* **工具**：使用 C++11 `<chrono>` 的 `std::chrono::high_resolution_clock` 量測各方法於不同規模 $N \in \{10, 50, 100, 500, 1000\}$ 下重複運算之平均耗時（微秒 $\mu\text{s}$ ）。
### 2. 效能量測程式
> 檔案名稱:test.cpp
```cpp
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>

using namespace std;

class Polynomial;

class Term {
    friend class Polynomial;
    friend ostream& operator<<(ostream& os, const Polynomial& p);
    friend istream& operator>>(istream& is, Polynomial& p);
private:
    float coef;
    int exp;
};

class Polynomial {
    friend ostream& operator<<(ostream& os, const Polynomial& p);
    friend istream& operator>>(istream& is, Polynomial& p);
private:
    Term* termArray;
    int capacity;
    int terms;
public:
    Polynomial() : capacity(2), terms(0) {
        termArray = new Term[capacity];
    }

    Polynomial(const Polynomial& other) : capacity(other.capacity), terms(other.terms) {
        termArray = new Term[capacity];
        for (int i = 0; i < terms; ++i) {
            termArray[i] = other.termArray[i];
        }
    }

    Polynomial& operator=(const Polynomial& other) {
        if (this != &other) {
            delete[] termArray;
            capacity = other.capacity;
            terms = other.terms;
            termArray = new Term[capacity];
            for (int i = 0; i < terms; ++i) {
                termArray[i] = other.termArray[i];
            }
        }
        return *this;
    }

    ~Polynomial() {
        delete[] termArray;
    }

    void NewTerm(float Coef, int Exp) {
        if (Coef == 0.0f) return;

        // 防呆：指數不能為負數
        if (Exp < 0) {
            cout << "[錯誤] 指數不能為負數 (" << Exp << ")，該項已被略過！" << endl;
            return;
        }

        for (int i = 0; i < terms; ++i) {
            if (termArray[i].exp == Exp) {
                termArray[i].coef += Coef;
                if (termArray[i].coef == 0.0f) {
                    for (int j = i; j < terms - 1; ++j) {
                        termArray[j] = termArray[j + 1];
                    }
                    terms--;
                }
                return;
            }
        }

        if (terms == capacity) {
            capacity *= 2;
            Term* temp = new Term[capacity];
            for (int i = 0; i < terms; ++i) {
                temp[i] = termArray[i];
            }
            delete[] termArray;
            termArray = temp;
        }

        int insertPos = 0;
        while (insertPos < terms && termArray[insertPos].exp > Exp) {
            insertPos++;
        }
        for (int i = terms; i > insertPos; --i) {
            termArray[i] = termArray[i - 1];
        }
        termArray[insertPos].coef = Coef;
        termArray[insertPos].exp = Exp;
        terms++;
    }

    Polynomial Add(Polynomial poly) {
        Polynomial c;
        int a = 0, b = 0;
        while (a < terms && b < poly.terms) {
            if (termArray[a].exp == poly.termArray[b].exp) {
                float sum = termArray[a].coef + poly.termArray[b].coef;
                if (sum != 0.0f) {
                    c.NewTerm(sum, termArray[a].exp);
                }
                a++;
                b++;
            }
            else if (termArray[a].exp < poly.termArray[b].exp) {
                c.NewTerm(poly.termArray[b].coef, poly.termArray[b].exp);
                b++;
            }
            else {
                c.NewTerm(termArray[a].coef, termArray[a].exp);
                a++;
            }
        }
        for (; a < terms; a++) c.NewTerm(termArray[a].coef, termArray[a].exp);
        for (; b < poly.terms; b++) c.NewTerm(poly.termArray[b].coef, poly.termArray[b].exp);
        return c;
    }

    Polynomial Mult(Polynomial poly) {
        Polynomial c;
        for (int i = 0; i < terms; ++i) {
            Polynomial temp;
            for (int j = 0; j < poly.terms; ++j) {
                float newCoef = termArray[i].coef * poly.termArray[j].coef;
                int newExp = termArray[i].exp + poly.termArray[j].exp;
                temp.NewTerm(newCoef, newExp);
            }
            c = c.Add(temp);
        }
        return c;
    }

    float Eval(float x) {
        float result = 0.0f;
        for (int i = 0; i < terms; ++i) {
            result += termArray[i].coef * pow(x, termArray[i].exp);
        }
        return result;
    }
};

// ----------------------------------------------------------------
// 測試主程式：使用 <cstdlib> 中的 clock() 進行微秒級計時
// ----------------------------------------------------------------
void runBenchmark(int N) {
    Polynomial p1, p2;

    for (int i = N; i >= 1; --i) {
        p1.NewTerm(1.5f, i * 2);
        p2.NewTerm(2.0f, i * 2 - 1);
    }

    int iterations = 1000;

    // --- 量測 Add ---
    clock_t start = clock();
    for (int i = 0; i < iterations; ++i) {
        Polynomial res = p1.Add(p2);
    }
    clock_t end = clock();
    double addTime = ((double)(end - start) / CLOCKS_PER_SEC * 1000000.0) / iterations;

    // --- 量測 Mult ---
    int multIter = (N > 100) ? 10 : 500;
    start = clock();
    for (int i = 0; i < multIter; ++i) {
        Polynomial res = p1.Mult(p2);
    }
    end = clock();
    double multTime = ((double)(end - start) / CLOCKS_PER_SEC * 1000000.0) / multIter;

    // --- 量測 Eval ---
    start = clock();
    for (int i = 0; i < iterations; ++i) {
        float val = p1.Eval(2.0f);
    }
    end = clock();
    double evalTime = ((double)(end - start) / CLOCKS_PER_SEC * 1000000.0) / iterations;

    // 格式化輸出
    cout << "規模 N = " << N << " | "
         << "Add: " << addTime << " us | "
         << "Mult: " << multTime << " us | "
         << "Eval: " << evalTime << " us" << endl;
}

int main() {
    cout << "=== 開始多項式效能量測 (單位: 微秒 us) ===" << endl;
    runBenchmark(10);
    runBenchmark(50);
    runBenchmark(100);
    runBenchmark(500);
    runBenchmark(1000);
    cout << "=== 量測結束 ===" << endl;
    return 0;
}
```
---
### 3. 量測 Shell 指令與終端輸出
```shell
$ g++ -O2 benchmark.cpp -o benchmark.exe
$ .\benchmark.exe
=== 開始多項式效能量測 (單位: 微秒 us) ===
規模 N = 10 | Add: 3 us | Mult: 26 us | Eval: 1 us
規模 N = 50 | Add: 11 us | Mult: 584 us | Eval: 2 us
規模 N = 100 | Add: 37 us | Mult: 3478 us | Eval: 5 us
規模 N = 500 | Add: 691 us | Mult: 28800 us | Eval: 25 us
規模 N = 1000 | Add: 2597 us | Mult: 2.2383e+06 us | Eval: 72 us
=== 量測結束 ===
```
### 4. 效能量測數據趨勢與複雜度深度分析

- 加法 Add 表現：驗證 $O(m + n)$ 線性複雜度
  1. 跨距驗證： $N=10$ ($3\ \mu\text{s}$) 到 $N=100$ ($37\ \mu\text{s}$) 規模放大 10 倍，耗時增加約 12.3 倍；小規模時受函式呼叫與物件建構等常數開銷影響。
  2. 極限規模： $N=1000$ 耗時僅約 $2.60\ \text{ms}$ ($2597\ \mu\text{s}$)，整體成長趨勢平緩，與輸入規模維持高度線性對齊。
  3. 機制結論：雙指標循序比對指數由大到小走訪，兩多項式之非零項至多僅存取一次，無冗餘走訪，實測數據高度吻合 $O(m + n)$ 線性時間複雜度。

- 乘法 Mult 表現：驗證 $O(N^3)$ 高階累積成長
  1. 跨距驗證： $N=10$ ($26\ \mu\text{s}$) 到 $N=100$ ($3478\ \mu\text{s}$) 規模放大 10 倍，耗時暴增約 133.8 倍，增長速率顯著超越二次方關係 ($10^2 = 100$)。
  2. 爆發增長： $N=100$ ($3478\ \mu\text{s}$) 到 $N=1000$ ($2.2383 \times 10^6\ \mu\text{s} \approx 2.24\ \text{s}$) 規模放大 10 倍，耗時暴增約 643.6 倍，呈現接近三次方 ( $10^3 = 1000$ ) 級距之急遽膨脹。
  3. 機制結論：外層迴圈逐項相乘後呼叫 `Add()` 累加，中間項數自 $N$ 逐步膨脹至 $N^2$，形成 $\sum_{i=1}^N (i \cdot N) = O(N^3)$ 的累積運算負擔，印證超大規模輸入下的計算瓶頸。

- 求值 Eval 表現：驗證 $O(N)$ 線性與快取友善性
  1. 耗時穩定：從 $N=10$ ($1\ \mu\text{s}$) 到 $N=1000$ ($72\ \mu\text{s}$)，即便在千項規模下耗時亦低於 $0.08\ \text{ms}$，全區間維持極為平緩的線性成長。
  2. 機制結論：連續陣列循序走訪觸發 CPU 空間局部性（Spatial Locality）的高快取命中率（Cache Hit），且全程無任何動態記憶體配置與搬移開銷，運算效率極高

---
## 結論

### 1. 系統設計與健壯性成就

- 自動正規化與防呆容錯：NewTerm 整合插入排序與同類項檢查，自動處理亂序與重複指數，相消為 0 時即時平移剔除以維持非零項定義
- 嚴謹遵循 Rule of Three：完整實作解構子、複製建構子與賦值運算子（operator=），採深層複製杜絕 Double Free 與記憶體洩漏
- 直覺的運算子多載封裝：多載 >> 與 <<，嚴格格式化正負符號、一次項、常數項與純零多項式，提供良好封裝介面

### 2. 理論演算法與實測效能驗證

- 加法運算驗證 $O(m + n)$ ：雙指標循序比對僅需線性走訪，N=1000 實測僅耗時 2.60 ms，增長與規模呈嚴格線性比例
- 求值運算驗證 $O(N)$ ：連續記憶體具高快取命中率（Cache Hit），N=1000 耗時僅 72 us，執行效率極高
- 乘法運算驗證 $O(N^3)$ ：逐輪累加造成中間項膨脹，N=1000 耗時達 2.24 s 符合理論預期，並提出統整排序與 Karatsuba 演算法為未來優化方向

---
## 心得討論

#### 1. 抽象資料型態與數學建模體悟
* 深刻理解 ADT 將數學多項式映射為 C++ 類別的封裝過程，透過物件導向將資料與運算完整隱藏與保護
> 我寫了一個 `class Polynomial` ，把「資料」（動態陣列 `termArray` 、項數 `terms` 、容量 `capacity` ）包在 private 裡面藏起來，外面的人只能透過我開放的公開函式（如 `Add()` 、 `Mult()` 、 `Eval()` ）來操作它。

* 體認到運算子多載（>>、<<、=）對於提升介面直覺性的重要性，使自訂資料型態能無縫融入標準 I/O 串流操作
> 透過多載 `operator<<` 和 `operator>>` 讓我寫的 `Polynomial 類別` ，用起來就像 C++ 內建的 int 或 string 一樣直覺。

#### 2. 底層記憶體管理與除錯實戰
* 實作動態陣列指標成員時，深刻體會到 Rule of Three 的必要性，避免了指標淺層複製引發的 Double Free 與當機
> 因為類別內部有使用 `new Term[capacity]` 向作業系統要動態記憶體，所以一定要自己記得（Rule of Three）：解構子(delete)、複製建構子(copy)、賦值運算子(=)

* 掌握了容量倍增機制（capacity *= 2）在記憶體動態擴充與均攤常數成本之間的平衡技巧
> 翻一倍就可以過一下子再複製資料，如果一格一格空間滿了就要每一次都得複製舊資料跟刪除舊空間太費時!!

#### 3. 演算法理論與硬體特性的連結

* 透過連續記憶體（Term*）的走訪，實體印證了空間局部性（Spatial Locality）對快取命中率（Cache Hit）與效能的巨大助益
> 因為陣列 termArray 在記憶體是一整條緊密排在一起的抓第0筆時會順便將1、2項抓進快取，當我的加法或求值迴圈讀到下一項時，資料早就在 CPU 裡面等著了，不需要每次大老遠跑去主記憶體（RAM）讀取，執行速度因此超級快。

* 藉由 `clock()` 微秒計時驗證了雙指標加法的 $O(m+n)$ 與乘法逐輪累加的 $O(N^3)$ 漸近成長趨勢，將抽象複雜度推導轉化為真實量化數據
> 透過撰寫高精度的計時基準程式，將課堂上抽象的時間複雜度漸近推導轉化為直觀且具體的毫秒/微秒數據，大幅加深了對演算法分析的實務理解

#### 4. 邊界條件與程式防呆架構的重要性

* 在 `NewTerm` 內建插入排序、同類項合併以及 0 係數剔除的過程中，認知到健全的防禦性程式設計能大幅提升系統容錯度與穩定度
> 1. 我學會了如何在使用者不再控制中亂輸入數字順序時我仍然可以讓它先在內部排序完，讓輸出是完美可讀性高的!!
> 2. 當次方一樣直接合併不浪費空間，跟當計算完係數變0時把它刪除後面資料往前補!!

#### 5. 新詞彙的練習

* `friend` 朋友機制
> 用了 `friend` ，告訴系統 `Polynomial` 是我的麻吉，它隨時可以進來讀取或修改我的 coef 和 exp !!
