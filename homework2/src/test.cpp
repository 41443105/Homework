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
