# 41443105
## Homework 2 作業 Polynomial
## 解題說明
本題要實作多項式抽象資料型態（Polynomial ADT）的相加、相乘與給 x 值的計算，並實作輸入和輸出重載。輸入可能包含重複指數、負係數或未排序的項，程式需要正確處理並輸出可閱讀的格式。多項式以降冪形式表示為各非零項之集合：

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
   - 擴充新項時，若相加後抵銷為 0 則剔除，自動進行同類項合併。
   - 插入新項確保內部陣列各項嚴格維持「指數由大到小降冪排列」，即使使用者亂輸入亦能自動修正。
   - 當空間額滿時執行容量加倍(*2)並重新配置記憶體，確保空間不會不夠。
3. **多項式加法（Add）**：採用雙指標（Two-Pointers）逐步走訪兩多項式的非零項：
   - 指數相同：係數相加，結果不為 0 則加入新多項式。
   - 指數不同：將指數較大者優先加入新多項式。
   - 雙指標比對完畢後，將剩餘未走訪的項目依序收尾追加。
4. **多項式乘法（Mult）**：使用雙重迴圈執行逐項相乘（係數相乘、指數相加），每完成一輪即呼叫 `Add()` 方法累加至結果多項式，自動合併同類項並維持降冪。
5. **記憶體管理**：自定義解構子（釋放動態空間）、複製建構子與賦值運算子（執行深層複製 Deep Copy），杜絕淺層複製導致的指標共用與 Double Free (a=b，a跟b指向同一個指標)記憶體錯誤。

---
## 程式實作

### IDE:
Microsoft Visual Studio Code C/C++

```cpp
#include <iostream>
#include <cmath>

using namespace std;

class Polynomial; // 前置宣告

class Term {

    friend class Polynomial;//Polynomial 是我的麻吉，它隨時可以進來讀取或修改我的 coef 和 exp

    //標準輸出/輸入串流的引用
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

    // 複製建構子 (深層複製，防止指標共用造成 double free)
    Polynomial(const Polynomial& other) : capacity(other.capacity), terms(other.terms) {
        termArray = new Term[capacity];
        for (int i = 0; i < terms; ++i) {
            termArray[i] = other.termArray[i];
        }
    }

    // 等於符號（=）
    Polynomial& operator=(const Polynomial& other) {
        if (this != &other) {  // 避免自己等於自己 ex:a = a
            delete[] termArray;
            capacity = other.capacity;
            terms = other.terms;
            termArray = new Term[capacity];
            for (int i = 0; i < terms; ++i) {
                termArray[i] = other.termArray[i];
            }
        }
        return *this;   // 回傳自己，以支援 (ex: c = b = a)
    }

    // 解構子: 釋放動態記憶體空間
    ~Polynomial() {
        delete[] termArray;
    }

    // 內部空間加倍擴容機制
    void NewTerm(float Coef, int Exp) {// 係數是 0 就不需要浪費空間儲存
        if (Coef == 0.0f) return;

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

        if (terms == capacity) { //空間滿了
            capacity *= 2;  //容量加倍
            Term* temp = new Term[capacity];
            for (int i = 0; i < terms; ++i) {
                temp[i] = termArray[i]; // 把舊元素搬進新抽屜
            }
            delete[] termArray; // 丟棄舊空間
            termArray = temp;
        }
        // 降冪排列
        // 尋找插入位置，確保次方由大到小排列，這樣輸出好看
        int updown = 0;
        while (updown < terms && termArray[updown].exp > Exp) {
            updown++;
        }

        // 將插入點之後的元素往後挪移一格
        for (int i = terms; i > updown; --i) {
            termArray[i] = termArray[i - 1];
        }

        termArray[updown].coef = Coef;
        termArray[updown].exp = Exp;
        terms++;
    }

    // 多項式加法: 回傳 *this + poly
    Polynomial Add(Polynomial poly) {
        Polynomial c;
        int a = 0, b = 0; // 兩個人的指標都從第 0 項 (最高次方) 開始比

        while (a < terms && b < poly.terms) {
            // 狀況 1：兩者次方一模一樣，把係數加起來
            if (termArray[a].exp == poly.termArray[b].exp) {
                float sum = termArray[a].coef + poly.termArray[b].coef;
                if (sum != 0.0f) {
                    c.NewTerm(sum, termArray[a].exp);
                }
                a++;
                b++;
            }
            // 狀況 2：對方的次方比較大，先把對方的放進結果
            else if (termArray[a].exp < poly.termArray[b].exp) {
                c.NewTerm(poly.termArray[b].coef, poly.termArray[b].exp);
                b++;
            }
            // 狀況 3：自己的次方比較大，先把自己的放進結果
            else {
                c.NewTerm(termArray[a].coef, termArray[a].exp);
                a++;
            }
        }
        // 當比完了將剩下來的項全部直接打包放進去
        for (; a < terms; a++) {
            c.NewTerm(termArray[a].coef, termArray[a].exp);
        }
        for (; b < poly.terms; b++) {
            c.NewTerm(poly.termArray[b].coef, poly.termArray[b].exp);
        }

        return c;
    }

    // 多項式乘法: 回傳 *this * poly
    Polynomial Mult(Polynomial poly) {
        Polynomial c;
        // 雙重迴圈：我方的每一項 (i) 配上對方的每一項 (j)
        for (int i = 0; i < terms; ++i) {
            Polynomial temp;
            for (int j = 0; j < poly.terms; ++j) {
                float newCoef = termArray[i].coef * poly.termArray[j].coef;
                int newExp = termArray[i].exp + poly.termArray[j].exp;
                temp.NewTerm(newCoef, newExp);
            }
            c = c.Add(temp);// 每乘完一輪，就呼叫Add() 自動合併同類項！
        }
        return c;
    }

    // 計算求值f(x)
    float Eval(float x) {
        float result = 0.0f;
        for (int i = 0; i < terms; ++i) {
            result += termArray[i].coef * pow(x, termArray[i].exp);
        }
        return result;
    }


};

// 多載 >> 運算子
// 輸入格式: [項數 n] [係數1] [指數1] [係數2] [指數2] ...
istream& operator>>(istream& is, Polynomial& p) {
    int n;
    if (!(is >> n)) return is;// 先讀第一個整數：總共有幾項

    delete[] p.termArray;  // 清空舊空間
    p.capacity = (n > 0) ? n : 2;
    p.terms = 0;
    p.termArray = new Term[p.capacity];

    // 接著一組一組讀入「係數」和「次方」
    for (int i = 0; i < n; ++i) {
        float c;
        int e;
        is >> c >> e;
        p.NewTerm(c, e);
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
        // 第一項如果是正數不印"+"，後面的項如果為正數印 "+"，負數印 "-"
        if (i > 0 && c > 0) {
            os << " + ";
        }
        else if (i > 0 && c < 0) {
            os << " - ";
            c = -c;
        }
        else if (i == 0 && c < 0) {
            os << "-";
            c = -c;
        }
        // 次方處理：次方為 0 不印 x；次方為 1 只印 x
        if (e == 0) {
            os << c;
        }
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

---

### 各成員函式與運算子複雜度彙整表

| 函式 / 運算子 | 時間複雜度 | 空間複雜度 | 演算法行為與空間配置說明 |
| :--- | :--- | :--- | :--- |
| **`Polynomial()`**<br>(預設建構子) | $O(1)$ | $O(1)$ | 配置常數初始容量（`capacity = 2`）之動態陣列，初始化成員變數。 |
| **`~Polynomial()`**<br>(解構子) | $O(1)$ | $O(1)$ | `Term` 無巢狀動態指標，直接以 `delete[] termArray` 釋放一整塊連續空間。 |
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
   - 採用雙指標（Two-Pointers）法比對兩多項式降冪陣列。由於每一輪迭代中至少有一個指標（`a` 或 `b`）向前遞增，因此總比對次數嚴格受限於兩多項式項數總和，時間複雜度為 $O(m + n)$ 。
   - 新建多項式最多只會儲存兩多項式全部項數（無同類項合併時），故額外輔助空間複雜度為 $O(m + n)$ 。

2. **多項式乘法 `Mult`**：
   - 外層迴圈共執行 $m$ 次，每次產生長度為 $n$ 的單項式積並呼叫一次 `Add()` 累加至結果物件。
   - 累加過程中的多項式項數逐步擴大：第 $1$ 次累加長度約為 $n$，第 $2$ 次約為 $2n$，至第 $m$ 次累加時規模達到 $mn$。總運算耗時為 $\sum_{i=1}^{m} (i \cdot n) = n \sum_{i=1}^{m} i = O(m^2 \cdot n)$ 。
   - 空間複雜度取決於展開後之非零項總數，最壞情況下所有乘積項次方皆不重複，空間複雜度為 $O(m \cdot n)$ 。

3. **輸入輸出運算子**：
   - `operator>>`：為了防呆並支援無序輸入，每次讀入皆透過 `NewTerm` 執行插入排序與同類項檢查。插入第 $i$ 項之平均時間為 $O(i)$ ，讀入 $n$ 項之總時間複雜度為 $O(n^2)$ ；動態陣列按需擴充，空間複雜度為 $O(n)$ 。
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
| 測試案例 | 輸入參數 | 預期輸出 | 實際輸出 |
|:---|:---|:---|:---|
| **測試一：多項式 P1(x)** | 4 | | |
| | 1 0 (常數項先輸) | | |
| | 2 1 (一次項) | | |
| | 3 2 (二次項) | | |
| | 1 1 (重複輸入一次項) | $3x^2 + 3x + 1$ | $3x^2 + 3x + 1$ |
| **測試二：多項式 P2(x)** | 3 | | |
| | -3 2 (負二次項) | | |
| | -3 1 (負一次項) | | |
| | 5 0 (常數項) | $-3x^2 - 3x + 5$ | $-3x^2 - 3x + 5$ |
| **$P1(x) + P2(x)$** | （執行加法 Add） | $6$ | $6$ |
| **$P1(x) \times P1(x)$** | （執行乘法 Mult） | $-9x^4 - 18x^3 + 3x^2 + 12x + 5$ | $-9x^4 - 18x^3 + 3x^2 + 12x + 5$ |
| **$P1(2)$** | （代入 $x = 2$ 執行 Eval） | $19$ | $19$ |
| **$P2(2)$** | （代入 $x = 2$ 執行 Eval） | $-13$ | $-13$ |

---


## 5. 申論及開發報告

### 1. 資料結構選用原因與效益分析
在多項式（Polynomial）的實作中，常見的方法有「以指數為索引的定長陣列」與「儲存非零項序對的動態陣列」兩種：
- **定長陣列的缺點**：若宣告一個大小為 $1000$ 的陣列來存多項式，當遇到稀疏多項式（Sparse Polynomial，如 $x^{1000} + 1$）時，中間九百多個位置皆填入 $0$，不僅浪費龐大記憶體空間，在執行加法時還必須遍歷所有為 $0$ 的元素，極度缺乏效率[cite: 7, 8]。
- **選用 `<coef, exp>` 動態陣列的優勢**：本實作採用 `Term* termArray` 搭配 `capacity` 與 `terms`，僅儲存有效之非零項[cite: 8]。此設計大幅降低記憶體開銷，並使多項式加法 `Add()` 的時間複雜度直接縮減至與實際非零項數相關的線性時間 $O(m + n)$[cite: 6, 7]。此外，搭配容量倍增機制（`capacity *= 2`），既兼顧了連續記憶體的快取友善性（Cache Friendly），又兼具動態擴充的彈性[cite: 8]。

---

### 2. C++ 記憶體管理與 Rule of Three 實踐
在本類別中，由於包含了底層指標成員 `Term* termArray` 進行動態記憶體配置（`new`），若未妥善處理生命週期，極易造成系統崩潰[cite: 8]：
1. **防止記憶體洩漏（Memory Leak）**：實作解構子 `~Polynomial()`，在物件生命週期結束離開作用域時主動呼叫 `delete[] termArray`，將資源歸還給作業系統[cite: 8]。
2. **避免淺層複製與 Double Free 當機**：若使用編譯器預設的複製行為，傳值呼叫（Pass by Value）時僅會複製指標位址（Shallow Copy），導致兩個多項式物件共用同一塊動態記憶體[cite: 8]。當其中一個物件先解構釋放記憶體後，另一個物件在解構時會觸發「同一記憶體釋放兩次（Double Free）」的嚴重錯誤。
3. **實作深層複製（Deep Copy）**：
   - 於**複製建構子**中重新 `new` 一塊大小相同的獨立空間，並逐項複製內容[cite: 8]。
   - 於**賦值運算子 `operator=`** 中先檢查是否自我賦值（`this != &other`），再釋放自身原有的舊陣列並配置新空間拷貝資料，確保了多項式在物件傳參、運算賦值（如 `c = c.Add(temp)`）時的記憶體安全性[cite: 7, 8]。

---

### 3. 開發過程中遭遇之問題與解決方案
在開發與測試階段，主要遭遇並克服了以下三項技術挑戰：

1. **使用者無序輸入與顯示雜亂問題**：
   - *問題*：多項式加法 `Add()` 依賴「指數由高到低降冪」的雙指標比對演算法[cite: 7]；若使用者未按順序輸入（例如先輸入低次方或常數項），不僅輸出格式不符數學慣例，更會導致加法雙指標比對邏輯失效[cite: 7, 11]。
   - *解法*：重構 `NewTerm()` 函式，引入插入排序思維[cite: 8]。每當有新項讀入時，循序搜尋適當的降冪插入點，將後續項目向後挪移一格再放入，保證多項式內部隨時維持嚴格降冪排列[cite: 7, 8]。
2. **重複指數與同類項自動合併**：
   - *問題*：使用者可能在輸入中分開鍵入相同次方的項（例如連續輸入兩個一次項）[cite: 11]。
   - *解法*：在 `NewTerm()` 插入前先走訪現有項，若已存在相同指數（`theExp`），則直接將係數累加進行同類項合併，避免陣列中出現冗餘項[cite: 7, 8, 11]。
3. **多項式相加係數抵銷為 0 的邊界處理**：
   - *問題*：當兩多項式相加剛好同次方正負抵銷（如 $3x^2$ 與 $-3x^2$ 相加為 $0$），若將係數為 $0$ 的項留存於陣列中，不僅違反 ADT 定義（僅存非零項），亦會造成後續輸出出現 `+ 0x^2` 的非預期結果[cite: 7, 8, 11]。
   - *解法*：在同類項合併與 `Add()` 邏輯中加入檢查，若係數相加等於 $0.0f$，則將該位置之後的所有元素往前搬移遞補並使 `terms--`，確保內部只維護真正的非零項[cite: 7, 8, 11]。當全部抵銷導致 `terms == 0` 時，`operator<<` 能正確輸出單一字元 `"0"`[cite: 9]。

---

### 4. 實作心得與成果
透過本次作業二的開發，深入理解了抽象資料型態（ADT）如何將數學模型具體轉化為 C++ 物件[cite: 7, 9]。除了掌握雙指標合併演算法外，更深刻體會到指標操作、動態記憶體配置及邊界條件防呆的重要性[cite: 7, 8]。藉由將運算子多載（`>>`、`<<`、`=`）完整融入類別中，讓多項式物件的操作如同內建基本型別般直覺流暢，提升了程式碼的可維護性與結構嚴謹度[cite: 7, 8, 9]。

### 結論

本次作業充分的練習到了物件導向,對多項式運算的邏輯化設計有更深理解。
此多項式系統成功實現了基礎的多項式四則運算，展示了 C++ 類別設計與動態陣列管理的應用。
程式結構清晰，擴充性高，適合作為教學與進階功能開發的基礎。
