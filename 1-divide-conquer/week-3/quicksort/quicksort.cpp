/**
 * Your task is to compute the total number of comparisons used to sort the given input file by QuickSort. As you know, the number of comparisons depends on which elements are chosen as pivots, so we'll ask you to explore three different pivoting rules.
 * You should not count comparisons one-by-one. Rather, when there is a recursive call on a subarray of length mm, you should simply add m-1 to your running total of comparisons. (This is because the pivot element is compared to each of the other m-1 elements in the subarray in this recursive call.)
 * WARNING: The Partition subroutine can be implemented in several different ways, and different implementations can give you differing numbers of comparisons. For this problem, you should implement the Partition subroutine exactly as it is described in the video lectures (otherwise you might get the wrong answer).
 * 
 * Problem 1
 * For the first part of the programming assignment, you should always use the first element of the array as the pivot element.
 * Problem 2
 * Compute the number of comparisons (as in Problem 1), always using the final element of the given array as the pivot element.
 * Problem 3
 * Compute the number of comparisons (as in Problem 1), using the "median-of-three" pivot rule.
 */

#include <iostream>
#include <fstream>
#include <dirent.h> // UNIX only; for Windows, need to get it elsewhere, see https://stackoverflow.com/a/612176/3477043
#include <string>
#include <map>

#include <array>
#include <vector>
#include <algorithm>

using namespace std;

#pragma region Utilities
template <typename T>
ostream &operator<<(ostream &os, vector<T> const &v)
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

bool verify(vector<int> const &sorted, vector<int> const &original)
{
   vector<int> correctAnswer = original;
   sort(correctAnswer.begin(), correctAnswer.end());

   for (int i = 0; i < sorted.size(); ++i)
      if (sorted[i] != correctAnswer[i])
         return false;

   return true;
}
#pragma endregion

enum ChoiceOfPivot
{
   FIRST,
   LAST,
   MEDIAN
};

int chooseMedianOfThree(int a, int b, int c, vector<int> const &v)
{
   array<int, 3> candidates = {a, b, c};
   sort(candidates.begin(), candidates.end(), [&](auto const x, auto const y) { return v[x] < v[y]; });
   return candidates[1]; // the median
}

int quickSort(int start, int end, vector<int> &v, ChoiceOfPivot const pivotChoice) // start is inclusive but end is EXCLUSIVE
{
   int n = end - start;

   if (n < 2)
      return 0;

   // Select the pivot
   int last = end - 1;
   int pivot_initial;
   switch (pivotChoice)
   {
   case LAST:
      pivot_initial = last;
      break;
   case MEDIAN:
      pivot_initial = chooseMedianOfThree(start, start + n / 2 - (n % 2 == 0 ? 1 : 0), last, v); // major note: NEVER forget to count FROM `start` and NOT implicitly from `0`!
      break;
   case FIRST:
   default:
      pivot_initial = start;
      break;
   }

   // Preprocess step: ensure that no matter which pivot element was picked, that it is placed at the start of the array; this is a no-op if pivot = start
   swap(v[start], v[pivot_initial]);

   // Partition around the pivot
   int j = start + 1; // marks the frontier between the partitioned section and the unpartitioned section
   int i = j;         // marks the frontier between the smaller elements and the larger elements, within the partitioned section
   while (j < end)
   {
      if (v[j] < v[start])
      {
         swap(v[i], v[j]);
         ++i;
      }

      ++j;
   }
   int pivot = i - 1;        // finally, identify the final location of the pivot...
   swap(v[start], v[pivot]); // ...and place it where it belongs

   // Recurse into both sides of the pivot
   return n - 1                                        // always doing comparisons equal to one less than the size of the array, see partition implementation above
          + quickSort(start, pivot, v, pivotChoice)    // before the pivot
          + quickSort(pivot + 1, end, v, pivotChoice); // after the pivot
}

int quickSort(vector<int> &v, ChoiceOfPivot const pivotChoice)
{
   return quickSort(0, v.size(), v, pivotChoice);
}

int main()
{
   // Find all test case files (courtesy of https://stackoverflow.com/a/612176/3477043)
   vector<string> test_case_files;
   DIR *dir;
   struct dirent *ent;
   string const TESTDIR = "./testcases";
   if ((dir = opendir(TESTDIR.c_str())) != nullptr)
   {
      while ((ent = readdir(dir)) != nullptr)
         if (ent->d_name[0] != '.')
            test_case_files.push_back(ent->d_name); // assumes that the file is not a dir
      closedir(dir);
   }
   else
   {
      // Could not open directory
      perror("");
      return EXIT_FAILURE;
   }

   // Sort them
   sort(test_case_files.begin(), test_case_files.end());

   // For each case, read the file and sort the input integers while computing the number of comparisons
   vector<pair<ChoiceOfPivot, string>> problems = {
       {ChoiceOfPivot::FIRST, "First"},
       {ChoiceOfPivot::LAST, "Last"},
       {ChoiceOfPivot::MEDIAN, "Median-of-Three"},
   };
   int const n_start = 1, n = test_case_files.size();
   for (auto it = test_case_files.begin() + n_start - 1; it != test_case_files.begin() + n_start - 1 + n; ++it)
   {
      string const &filename = *it;
      string const filepath = TESTDIR + "/" + filename;
      vector<int> test_case;
      ifstream file(filepath);
      int x;
      while (file >> x)
         test_case.push_back(x);

      cout << filename << ":" << endl;
      for (auto const &problem : problems)
      {
         cout << problem.second << ";";
         auto sorted = test_case; // make a copy
         int comparisons = quickSort(sorted, problem.first);
         bool sortedCorrectly = verify(sorted, test_case);
         if (!sortedCorrectly)
            cout << "(FAILED!)";
         else
            cout << comparisons;
         cout << endl;
      }
      cout << "; " << endl;
   }

   return 0;
}