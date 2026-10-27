#include <iostream>
#include <cmath>
#include <chrono> // 1. 引入高精度計時函式庫

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
// 2. 測試主程式：自動生成資料並計時
// ----------------------------------------------------------------
void runBenchmark(int N) {
    Polynomial p1, p2;

    // 自動建構規模為 N 的多項式（降冪排列，指數為 2*i 與 2*i+1）
    for (int i = N; i >= 1; --i) {
        p1.NewTerm(1.5f, i * 2);      // 例如: 1.5x^10, 1.5x^8...
        p2.NewTerm(2.0f, i * 2 - 1);  // 例如: 2.0x^9, 2.0x^7...
    }

    int iterations = 1000; // 跑 1000 次取平均，避免單次時間太短量不到

    // --- 量測 Add ---
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        Polynomial res = p1.Add(p2);
    }
    auto end = chrono::high_resolution_clock::now();
    double addTime = chrono::duration<double, micro>(end - start).count() / iterations;

    // --- 量測 Mult ---
    // (如果 N 太大，Mult 跑 1000 次會很久，可以自動縮減次數)
    int multIter = (N > 100) ? 50 : 500;
    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < multIter; ++i) {
        Polynomial res = p1.Mult(p2);
    }
    end = chrono::high_resolution_clock::now();
    double multTime = chrono::duration<double, micro>(end - start).count() / multIter;

    // --- 量測 Eval ---
    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        float val = p1.Eval(2.0f);
    }
    end = chrono::high_resolution_clock::now();
    double evalTime = chrono::duration<double, micro>(end - start).count() / iterations;

    // 輸出結果
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
