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
        }
        else {
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
