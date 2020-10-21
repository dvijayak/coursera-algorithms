#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <cmath>

using namespace std;

int ctoi (char c)
{
  return c - '0';
}

char itoc (int i)
{
  return '0' + i;  
}

string add (string const& a, string const& b)
{
  int length_a = a.length();
  int length_b = b.length();
  if (length_b > length_a)
    return add(b, a);

  vector<char> result;
  int carry = 0;
  for (int i_a = length_a-1, i_b = length_b-1; i_a >= 0; --i_a, --i_b)
  {
    int v_a = ctoi(a[i_a]);
    int v = carry + v_a;
    if (i_b >= 0)
    {
      int v_b = ctoi(b[i_b]);
      v += v_b;
    }

    carry = v/10;
    int digit = v - carry*10;
    result.push_back(itoc(digit));
  }  

  stringstream ss;
  for (auto it = result.rbegin(); it != result.rend(); ++it)
    ss << *it;
  return ss.str();
}

void padZero (string & s, int const n)
{
  if (n <= 0)
    return;

  for (int i = 0; i < n; ++i)
    s.push_back('0');
}

void padZeroPrefix (string & s, int const n)
{
  if (n <= 0)
    return;

  for (int i = 0; i < n; ++i)
    s.insert(s.begin(), '0');
}

string multiplyNumbersOfSameLengthPow2 (string const& x, string const& y)
{
  int n = x.length();

  // Base case
  if (n == 1)
  {
    int product = ctoi(x[0]) * ctoi(y[0]);
    stringstream ss;
    ss << product;
    return ss.str();
  }

  // Else, divide and conquer
  int n2 = n / 2;
  string a = x.substr(0, n2);
  string b = x.substr(n2, n2);
  string c = y.substr(0, n2);
  string d = y.substr(n2, n2);

  string ac = multiplyNumbersOfSameLengthPow2(a, c);
  string bd = multiplyNumbersOfSameLengthPow2(b, d);
  // Normally, you'd do ad + bc = (a+b)(c+d) - ac - bd, but I don't want to bother with implementing subtraction for my string-based big-int representation, so I'll just stick to the 4 multiplication version
  string ad = multiplyNumbersOfSameLengthPow2(a, d);
  string bc = multiplyNumbersOfSameLengthPow2(b, c);
  string ad_plus_bc = add(ad, bc);

  // Pad
  padZero(ac, n);
  padZero(ad_plus_bc, n2);

  // Add
  return add(ac, add(ad_plus_bc, bd));  
}

string multiply (string const& x, string const& y)
{
  int length_x = x.length();
  int length_y = y.length();

  if (length_x < length_y)
    return multiply(y, x);

  // Pad up to the next power of 2
  string x_padded = x;
  int zerosToPadForPow2 = int(pow(2, round(log2(length_x)))) - length_x;
  padZeroPrefix(x_padded, zerosToPadForPow2);
  string y_padded = y;
  padZeroPrefix(y_padded, x_padded.length() - length_y);

  // Compute the result
  string result = multiplyNumbersOfSameLengthPow2(x_padded, y_padded);

  // Remove leading zeros
  int unpaddedStart = 0;
  for (int i = 0; i < result.length(); ++i)
  {
    unpaddedStart = i;
    if (result[i] != '0')
      break;
  }
  return result.substr(unpaddedStart, string::npos);
} 

int main() {
  map<pair<string, string>, string> testCases = {
    { {"2", "2"}, "4"},
    { {"3", "4"}, "12"},
    { {"9", "3"}, "27"},
    { {"2", "10"}, "20"},
    { {"10", "3"}, "30"},
    { {"3", "10"}, "30"},
    { {"1054", "231"}, "243474"},
    { {"1234", "5678"}, "7006652"},
    { {"5678", "1234"}, "7006652"},
    { {"1950293", "529503"}, "1032685994379"},
    { {"12345678", "87654"}, "1082148059412"},
  };
  
  for (auto const& testCase : testCases)
  {
    string const& a = testCase.first.first;
    string const& b = testCase.first.second;
    string result = multiply(a, b);
    string const& expected = testCase.second;
    bool success = result == expected;
    cout << a << " * " << b << " = " << result << (success ? "" : " (FAIL! ");
    if (!success)
      cout << "Expected " << expected << ")";
    cout << endl;
  }

  // Assignment question  
  {
    string x = "3141592653589793238462643383279502884197169399375105820974944592";
    string y = "2718281828459045235360287471352662497757247093699959574966967627";
    cout << "Assignment Q: " << x << " * " << y << " = " << multiply(x, y) << endl;
  }
}
