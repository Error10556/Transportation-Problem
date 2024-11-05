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

template <class T> struct TransportationProblemSetup;

template <class T> class InitialSolutionStep
{
public:
    typedef bool (*Func)(TransportationProblemSetup<T>&);
};

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
    void Solve(typename InitialSolutionStep<T>::Func solver)
    {
        while (solver(*this))
            ;
    }
    Matrix<string> Render() const
    {
        int w = costs.Width(), h = costs.Height();
        Matrix<string> table(2 + h, 2 + w);
        table[0][0] = "Source\\Dest";
        for (int i = 0; i < w; i++)
        {
            string& cur = table[0][i + 1] = to_string(i + 1);
            if (DemandIsClosed(i))
                cur.push_back('*');
        }
        for (int i = 0; i < h; i++)
        {
            string& cur = table[i + 1][0] = to_string(i + 1);
            if (SupplyIsClosed(i))
                cur.push_back('*');
        }
        table[h + 1][0] = "Demand";
        table[0][w + 1] = "Supply";
        for (int i = 0; i < supply.size(); i++)
            table[i + 1][w + 1] =
                "(" + ConvertToString(supply[i]) + ") " + ConvertToString(initsupply[i]);
        for (int i = 0; i < demand.size(); i++)
            table[h + 1][i + 1] =
                "(" + ConvertToString(demand[i]) + ") " + ConvertToString(initdemand[i]);
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++)
                if (basic[i][j])
                    table[i + 1][j + 1] = ConvertToString(basicVal[i][j]) +
                                          " ($" + ConvertToString(costs[i][j]) +
                                          ")";
        return table;
    }
};

template <class T>
ostream& operator<<(ostream& out, const TransportationProblemSetup<T>& tp)
{
    return out << tp.Render();
}

template <class T> bool NorthwestCornerRule(TransportationProblemSetup<T>& tp)
{
    int col, row;
    int w = tp.Demand().size();
    int h = tp.Supply().size();
    for (col = 0; col < w; col++)
        if (!tp.DemandIsClosed(col))
            break;
    if (col == w)
        return false;
    for (row = 0; row < h; row++)
        if (!tp.SupplyIsClosed(row))
            break;
    if (row == h)
        return false;
    tp.ChooseAsBasic(row, col);
    return true;
}

template <class T> pair<T, T> TwoMin(const vector<T>& items)
{
    if (items.size() == 1)
        return {items[0], items[0]};
    T a = items[0], b = items[1];
    if (a > b)
        swap(a, b);
    for (int i = 2; i < items.size(); i++)
    {
        T cur = items[i];
        if (cur < a)
        {
            b = a;
            a = cur;
        }
        else
            b = min(b, cur);
    }
    return {a, b};
}

template <class T> bool VogelApproximation(TransportationProblemSetup<T>& tp)
{
    vector<pair<T, int>> live;
    int w = tp.Demand().size();
    int h = tp.Supply().size();
    int bestdiff = -1;
    int bestrow = -1, bestcol = -1;
    const auto& costs = tp.Costs();
    for (int i = 0; i < w; i++)
    {
        if (tp.DemandIsClosed(i))
            continue;
        live.clear();
        for (int j = 0; j < h; j++)
            if (!tp.IsBasic(j, i) && !tp.SupplyIsClosed(j))
                live.emplace_back(costs[j][i], j);
        if (live.empty())
            continue;
        auto p = TwoMin(live);
        int diff = p.second.first - p.first.first;
        if (bestdiff < diff)
        {
            bestcol = true;
            bestdiff = diff;
            bestcol = i;
            bestrow = p.first.second;
        }
    }
    for (int i = 0; i < h; i++)
    {
        if (tp.SupplyIsClosed(i))
            continue;
        live.clear();
        for (int j = 0; j < w; j++)
            if (!tp.IsBasic(i, j) && !tp.DemandIsClosed(j))
                live.emplace_back(costs[i][j], j);
        if (live.empty())
            continue;
        auto p = TwoMin(live);
        int diff = p.second.first - p.first.first;
        if (bestdiff < diff)
        {
            bestcol = false;
            bestdiff = diff;
            bestrow = i;
            bestcol = p.first.second;
        }
    }
    if (bestdiff == -1)
        return false;
    tp.ChooseAsBasic(bestrow, bestcol);
    return true;
}

int main()
{
    vector<int> sup{50, 60, 50, 50};
    vector<int> dem{30, 20, 70, 30, 60};
    Matrix<int> cost(4, 5);
    const int M = 1000;
    cost[0] = {16, 16, 13, 22, 17};
    cost[1] = {14, 14, 13, 19, 15};
    cost[2] = {19, 19, 20, 23, M};
    cost[3] = {M, 0, M, 0, 0};
    TransportationProblemSetup ts(sup, dem, cost);
    ts.Solve(VogelApproximation<int>);
    cout << ts;
    cout << "Total: " << ts.TotalCost();
}
