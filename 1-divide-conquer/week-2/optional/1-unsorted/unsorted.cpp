/**
 * 
 * You are given as input an unsorted array of n distinct numbers, where n is a power of 2. Give an algorithm that identifies the second-largest number in the array, and that uses at most n + lg(n) - 2n comparisons.
 */

#include <iostream>

#include <limits>
#include <vector>
#include <array>
#include <algorithm>

using namespace std;

#pragma region Utilities
bool verify(vector<int> const &v, int actual)
{
   int n = v.size();

   if (n < 2)
      return actual == v[0];

   vector<int> sorted = v;
   sort(sorted.begin(), sorted.end());
   return actual == sorted[n - 2];
}

ostream &operator<<(ostream &os, vector<int> const &v)
{
   os << "[";
   for (int i = 0; i < v.size(); ++i)
   {
      os << v[i];
      if (i != v.size() - 1)
         os << ", ";
   }
   os << "]";

   return os;
}
#pragma endregion



typedef array<int, 2> TwoInts;

/**
 * Number of comparisons analysis when n is a power of 2:
 * - The height of the recursion tree will be lg(n) - 1, since the base case kicks in when n = 2.
 * - The base case has exactly 1 comparison.
 * - The "merge" or Conquer/Combine step has exactly 2 comparisons.
 * - So the total number of comparisons will be 2*(2^lg(n) - 1) - 2^lg(n)-1
 * This is definitely better than the straightforward approach which takes 2*n comparisons, but it isn't meet the problem expectation.
 */
TwoInts findFirstTwoLargest(vector<int> const& v, int s, int e) // Note: the range is [s, e), that is inclusive of s, but exclusive of e
{
   TwoInts result;

   int n = e - s;
   if (n == 1)
   {
      result[0] = v[s];
      result[1] = std::numeric_limits<int>::min();
   }
   else if (n == 2)
   {
      // Base case
      int first = v[s];
      int second = v[s + 1];

      if (first > second)
      {
         result[0] = first;
         result[1] = second;
      }
      else
      {
         result[0] = second;
         result[1] = first;
      }
   }
   else
   {
      // Divide
      int mid = s + n / 2;
      auto result_half1 = findFirstTwoLargest(v, s, mid);
      auto result_half2 = findFirstTwoLargest(v, mid, e);

      // Conquer
      // Using the "merge" step of merge sort, compute the first two largest numbers in descending sorted order from the results of the divide calls above
      for (int i = 0, j = 0, k = 0; k < 2; k++)
      {
         int *choice;
         if (result_half1[i] > result_half2[j])
         {
            choice = &result_half1[i];
            i++;
         }
         else
         {
            choice = &result_half2[j];
            j++;
         }

         result[k] = *choice;
      }
   }

   return result;
}

int findSecondLargest(vector<int> const& v)
{
   int n = v.size();

   if (n < 2)
      return v[0];

   auto firstTwo = findFirstTwoLargest(v, 0, n);
   return firstTwo[1];
}

int main()
{
   vector<vector<int>> test_cases = {
      {8, 4, 5, 4, 9, 2, 0, 1},
      {9, 1, 90, 20, 23},
      {9},
      {-5, 4, 14, 6, 17, -2, 3, -11, 20, -12},
      {-1, 20, 19, 15, -13, -11, -15, 4, -18, 9},
      {-15, -1, -14, 14, -10, -9, -11, 16, -13, 1},
      {20, -9, -19, 9, 15, -12, 16, -16, -10, 10},
      {-1, -17, 19, 10, -8, -15, 4, -3, -18, -10},
      {17, 18, -6, 7, -9, -15, -1, -2, 16, 1},
      {9, 13, -4, -18, -8, -16, 1, 18, 14, 15},
      {-12, 9, -13, -2, -15, -1, 1, -6, 4, -14},
      {-10, -19, -8, 13, -6, 0, 10, -20, -16, -1},
      {10, 7, 4, 5, -6, -18, -13, 11, -12, 9},
   };

   for (auto &test_case : test_cases)
   {
      int result = findSecondLargest(test_case);
      bool success = verify(test_case, result);
      if (!success)
         cout << "(FAILED!) ";
      cout << "Result = " << result << ", Input = " << test_case;
      cout << endl;
   }

   return 0;
}