#include <algorithm>
#include <iostream>
#include <numeric>
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

template <class T> ostream& operator<<(ostream& out, const vector<T>& v)
{
    out << "{ ";
    bool notfirst = false;
    for (const auto& i : v)
    {
        if (notfirst)
            out << ", ";
        out << i;
        notfirst = true;
    }
    return out << " }";
}

template <class T, class U>
ostream& operator<<(ostream& out, const pair<T, U>& p)
{
    return out << '(' << p.first << ", " << p.second << ')';
}

template <class T> struct TransportationProblemSetup
{
private:
    vector<T> supply, demand, initsupply, initdemand;
    Matrix<T> costs;
    Matrix<bool> basic;
    Matrix<T> basicVal;
    vector<bool> supplyClosed, demandClosed;
    T totalcost = 0;

public:
    TransportationProblemSetup(const vector<T>& supply, const vector<T>& demand,
                               const Matrix<T>& costs)
        : supply(supply), initsupply(supply), demand(demand),
          initdemand(demand), costs(costs),
          basic(costs.Height(), costs.Width()),
          basicVal(costs.Height(), costs.Width()), supplyClosed(supply.size()),
          demandClosed(demand.size())
    {
    }
    void ChooseAsBasic(int row, int col)
    {
        T& sup = supply[row];
        T& dem = demand[col];
        T inc;
        if (sup <= dem)
        {
            inc = sup;
            supplyClosed[row] = true;
        }
        else
        {
            inc = dem;
            demandClosed[col] = true;
        }
        sup -= inc;
        dem -= inc;
        basicVal[row][col] = inc;
        basic[row][col] = true;
        totalcost += inc * costs[row][col];
    }
    const vector<T>& Supply() const
    {
        return initsupply;
    }
    const vector<T>& Demand() const
    {
        return initdemand;
    }
    const vector<T>& RemainingSupply() const
    {
        return supply;
    }
    const vector<T>& RemainingDemand() const
    {
        return demand;
    }
    const Matrix<T>& Costs() const
    {
        return costs;
    }
    const Matrix<T>& BasicValues() const
    {
        return basicVal;
    }
    T BasicValue(int row, int col) const
    {
        return basicVal[row][col];
    }
    bool IsBasic(int row, int col) const
    {
        return basic[row][col];
    }
    const Matrix<bool>& Basics() const
    {
        return basic;
    }
    bool SupplyIsClosed(int row) const
    {
        return supplyClosed[row];
    }
    bool DemandIsClosed(int col) const
    {
        return demandClosed[col];
    }
    const vector<bool>& ClosedSupplies() const
    {
        return supplyClosed;
    }
    const vector<bool>& ClosedDemands() const
    {
        return demandClosed;
    }
    T TotalCost() const
    {
        return totalcost;
    }
    bool Balanced() const
    {
        return accumulate(supply.begin(), supply.end(), 0) ==
               accumulate(demand.begin(), demand.end(), 0);
    }
};

template <class T>
ostream& operator<<(ostream& out, const TransportationProblemSetup<T>& tp)
{
    const auto& costs = tp.Costs();
    const auto& basics = tp.Basics();
    const auto& basicVals = tp.BasicValues();
    int w = costs.Width(), h = costs.Height();
    Matrix<string> table(2 + h, 2 + w);
    table[0][0] = "Source\\Dest";
    for (int i = 0; i < w; i++)
    {
        string& cur = table[0][i + 1] = to_string(i + 1);
        if (tp.DemandIsClosed(i))
            cur.push_back('*');
    }
    for (int i = 0; i < h; i++)
    {
        string& cur = table[i + 1][0] = to_string(i + 1);
        if (tp.SupplyIsClosed(i))
            cur.push_back('*');
    }
    table[h + 1][0] = "Demand";
    table[0][w + 1] = "Supply";
    const auto& sup = tp.Supply();
    const auto& dem = tp.Demand();
    for (int i = 0; i < sup.size(); i++)
        table[i + 1][w + 1] = ConvertToString(sup[i]);
    for (int i = 0; i < dem.size(); i++)
        table[h + 1][i + 1] = ConvertToString(dem[i]);
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
            if (basics[i][j])
                table[i + 1][j + 1] = "(" + ConvertToString(costs[i][j]) +
                                      ") " + ConvertToString(basicVals[i][j]);
    return out << table;
}

int main()
{
}
