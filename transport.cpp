#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

template <class T> string ConvertToString(T&& obj)
{
    ostringstream ss;
    ss << obj;
    return ss.str();
}

template <class T> struct Matrix
{
private:
    vector<vector<T>> mat;

public:
    Matrix(int h, int w) : mat(h, vector<T>(w))
    {
    }
    void Resize(int h, int w)
    {
        mat.resize(h);
        for (auto& v : mat)
            v.resize(w);
    }
    vector<T>& operator[](size_t index)
    {
        return mat[index];
    }
    const vector<T>& operator[](size_t index) const
    {
        return mat[index];
    }
    size_t Width() const
    {
        return mat.empty() ? 0 : mat[0].size();
    }
    size_t Height() const
    {
        return mat.size();
    }
    vector<T> Column(size_t index) const
    {
        vector<T> res(Height());
        for (int i = 0; i < res.size(); i++)
            res[i] = mat[i][index];
        return res;
    }
    Matrix<string> ToStrings() const
    {
        int n = Height(), m = Width();
        Matrix<string> table(n, m);
        for (int i = 0; i < n; i++)
        {
            vector<string>& row = table[i];
            auto& myrow = mat[i];
            for (int j = 0; j < m; j++)
                row[j] = ConvertToString(myrow[j]);
        }
        return table;
    }
};

ostream& operator<<(ostream& out, const Matrix<string>& mat)
{
    int w = mat.Width(), h = mat.Height();
    vector<size_t> widths(w);
    for (int i = 0; i < w; i++)
    {
        size_t& cur = widths[i];
        cur = 0;
        for (int j = 0; j < h; j++)
            cur = max(cur, mat[j][i].size());
    }
    for (int i = 0; i < h; i++)
    {
        auto& row = mat[i];
        for (int j = 0; j < w; j++)
        {
            if (j)
                out << "  ";
            const string& s = row[j];
            size_t space = widths[j] - s.size();
            for (int k = space / 2; k; --k)
                out.put(' ');
            out << s;
            for (int k = (space + 1) / 2; k; --k)
                out.put(' ');
        }
        out << '\n';
    }
    return out;
}

template <class T> ostream& operator<<(ostream& out, const Matrix<T>& mat)
{
    return out << mat.ToStrings();
}

template <class T> struct TransportationProblem
{
public:
    vector<T> supply, demand;
    Matrix<T> costs;
    Matrix<bool> basic;
    Matrix<T> basicVal;

};

int main()
{
    Matrix<int> a(2, 3);
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 3; j++)
            a[i][j] = (j + 8 + (j + 1) * (i + 1));
    cout << a;
}
