#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
#include <iostream>

using namespace std;

class BigNumber
{
private:
    vector<int> digits;
    bool state;
public:
    BigNumber()
        :BigNumber(0) {}
    BigNumber(vector<int> aux, bool st)
        :digits(aux), state(st) {}
    BigNumber(int64_t x);
    BigNumber(string s);
    friend ostream& operator<<(ostream& os, const BigNumber& num);
    friend void operator>>(istream& is, BigNumber& num);
    vector<int> get_digits() const
    {
        return digits;
    }
    bool get_state() const
    {
        return state;
    }
    void set_digits(vector<int> aux)
    {
        this->digits = aux;
    }
    void set_state(bool ns)
    {
        this->state = ns;
    }

    BigNumber operator-(const BigNumber& rhs) const;
    BigNumber operator+(const BigNumber& rhs) const;
    BigNumber operator*(const BigNumber& rhs) const;
    BigNumber operator/(const BigNumber& rhs) const;

    bool operator<(const BigNumber& rhs) const {
        if (digits.size() < rhs.digits.size())
            return true;
        if (digits.size() > rhs.digits.size())
            return false;
        for (size_t i = rhs.digits.size() - 1; i >= 0; i--)
        {
            if (rhs.digits[i] > digits[i])
                return true;
            if (rhs.digits[i] < digits[i])
                return false;
        }
        return false;
    }

    bool operator>(const BigNumber& rhs) const {
        if (digits.size() > rhs.digits.size())
            return true;
        if (digits.size() < rhs.digits.size())
            return false;
        size_t i = digits.size() - 1;
        while (i >= 0)
        {
            if (rhs.digits.at(i) > digits.at(i))
                return false;
            if (rhs.digits.at(i) < digits.at(i))
                return true;
            i--;
        }
        return false;
    }

    bool operator==(const BigNumber& rhs) const {
        if (digits.size() != rhs.digits.size())
            return false;
        for (int i = rhs.digits.size() - 1; i >= 0; i--)
            if (rhs.digits[i] != digits[i])
                return false;
        return true;
    }

    bool operator!=(const BigNumber& rhs) const {
        if (digits.size() != rhs.digits.size())
            return true;
        if (*this == rhs)
            return false;
        return true;
    }

    bool operator>=(const BigNumber& rhs) const {
        if (*this > rhs || *this == rhs)
            return true;
        return false;
    }

    bool operator<=(const BigNumber& rhs) const {
        if (*this < rhs || *this == rhs)
            return true;
        return false;
    }
};

ostream& operator<<(ostream& os, const BigNumber& num)
{
    string redo;
    auto digits = num.get_digits();
    if (!digits.size())
        return os << '0';
    if (!num.get_state())
        redo += '-';
    for_each(digits.rbegin(), digits.rend(), [&redo](auto digit) {
        redo += to_string(digit);
        });
    return os << redo;
}

void operator>>(istream& is, BigNumber& num)
{
    string in;
    vector<int> aux;
    is >> in;
    num.set_state(true);
    if (in[0] == '-')
    {
        num.set_state(false);
        in.erase(in.begin());
    }
    for_each(in.rbegin(), in.rend(), [&aux](auto digit) {
        aux.push_back(digit - '0');
        });
    num.set_digits(aux);
}

BigNumber::BigNumber(int64_t x) {
    state = x >= 0;
    if (x < 0)
        x *= -1;
    while (x)
    {
        digits.push_back(x % 10);
        x /= 10;
    }
}

BigNumber::BigNumber(string s) {
    this->state = true;
    if (s[0] == '-')
    {
        this->state = false;
        s.erase(s.begin());
    }
    vector<int> aux;
    for_each(s.rbegin(), s.rend(), [&aux](auto digit) {
        aux.push_back((int)(digit - '0'));
        });
    this->digits = aux;
}

BigNumber BigNumber::operator+(const BigNumber& rhs) const {
    if (!state && rhs.state) //- + => + -
        return rhs - BigNumber(digits, true);
    if (state && !rhs.state) //+ - => minus
        return *this - BigNumber(rhs.digits, true);
    BigNumber result;
    int i = 0, j = 0, carry = 0;
    const int n = digits.size(), m = rhs.digits.size();
    while (i < n || j < m || carry)
    {
        int sum = carry;
        if (i < n)
            sum += digits[i++];
        if (j < m)
            sum += rhs.digits[j++];
        carry = sum / 10;
        result.digits.push_back(sum % 10);
    }
    result.state = true;
    if (!state && !rhs.state)
        result.state = false;
    return result;
}

BigNumber BigNumber::operator-(const BigNumber& rhs) const {
    if (!state && rhs.state) //- -(+) => -(+ +)
        return *this + BigNumber(rhs.digits, false);
    if (!state && !rhs.state) //- -(-) => s+ - f+
        return BigNumber(rhs.digits, true) - BigNumber(digits, true);
    if (state && !rhs.state) //+ -(-) => + +
        return *this + BigNumber(rhs.digits, true);
    if (state == rhs.state && *this == rhs) //same <3
        return 0;
    BigNumber result;
    vector<int> lhsv, rhsv;
    if (*this > rhs)
    {
        lhsv = digits;
        rhsv = rhs.digits;
        result.state = true;
    }
    else
    {
        lhsv = rhs.digits;
        rhsv = digits;
        result.state = false;
    }
    for (size_t i = 0; i < lhsv.size(); i++)
    {
        if (i < rhsv.size())
        {
            if (lhsv[i] >= rhsv[i])
                result.digits.push_back(lhsv[i] - rhsv[i]);
            else
            {
                size_t j = i + 1;
                while (lhsv[j] == 0)
                    lhsv[j++] = 9;
                lhsv[j]--;
                result.digits.push_back(10 + lhsv[i] - rhsv[i]);
            }
        }
        else
            result.digits.push_back(lhsv[i]);
    }
    while (result.digits.back() == 0)
        result.digits.pop_back();
    return result;
}

BigNumber BigNumber::operator*(const BigNumber& rhs) const
{
    BigNumber result;
    result.state = (state == rhs.state);
    result.digits.resize(digits.size() + rhs.digits.size(), 0);
    for (int i = 0; i < digits.size(); ++i)
    {
        int carry = 0;
        for (int j = 0; j < rhs.digits.size() || carry; ++j)
        {
            long long sum = result.digits[i + j] + carry;
            if (j < rhs.digits.size())
                sum += digits[i] * rhs.digits[j];
            carry = sum / 10;
            result.digits[i + j] = sum % 10;
        }
    }
    while (result.digits.size() > 1 && result.digits.back() == 0)
        result.digits.pop_back();
    return result;
}

BigNumber BigNumber::operator/(const BigNumber& rhs) const 
{
    if (rhs.digits.size() == 0)
        return -1; //nuh-uh >:(
    if (*this == rhs)
        return 1; //in my dreams
    if (*this < rhs)
        return 0; //I will NOT aproximate this.
    BigNumber result = 1;
    result.state = true;
    BigNumber aux = BigNumber(digits, true);
    BigNumber temp = BigNumber(rhs.digits, true);
    while (aux - temp >= BigNumber({ 0 }, true))
    {
        aux = aux - temp;
        result = result + BigNumber({ 1 }, true);
    }
    result.state = (state == rhs.state);
    return result;
}

BigNumber f(int n)
{
    if (n <= 0)
        return 0;
    if (n == 1)
        return 1;
    vector<BigNumber> fibo(n);
    fibo[0] = BigNumber(1);
    fibo[1] = BigNumber(1);
    for (int i = 2; i < n; ++i)
        fibo[i] = fibo[i - 1] + fibo[i - 2];
    return fibo[n - 1];
}

int main()
{
    BigNumber x, y;
    int n;
    cin >> x;
    cin >> y;
    cout << x + y << '\n' << x - y << '\n' << x * y << '\n' << x / y << '\n';
    cin >> n;
    cout << f(n);
}