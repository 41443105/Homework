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
