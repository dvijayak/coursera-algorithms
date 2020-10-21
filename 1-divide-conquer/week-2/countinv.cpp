#include <iostream>
#include <fstream>
#include <dirent.h> // UNIX only; for Windows, need to get it elsewhere, see https://stackoverflow.com/a/612176/3477043

#include <vector>
#include <algorithm>

using namespace std;

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

// Merge the sorted subarrays specified by the ranges [start_a, end_a] and [start_b, end_b]
// Assumption: the subarrays are contiguous, i.e. if you iterate from start_a to end_b inclusive you will have traversed both subarrays
size_t mergeAndCountInversions(vector<int> &v, int start_a, int end_a, int start_b, int end_b)
{
   size_t count = 0;
   vector<int> scratch;

   int a = start_a, b = start_b;
   for (; a <= end_a && b <= end_b;)
   {
      if (v[a] <= v[b])
      {
         scratch.push_back(v[a]);
         a++;
      }
      else
      {
         scratch.push_back(v[b]);
         b++;

         count += end_a + 1 - a;
      }
   }
   {
      int start = 0, end = 0;
      if (a <= end_a)
      {
         start = a;
         end = end_a + 1;
      }
      else if (b <= end_b)
      {
         start = b;
         end = end_b + 1;
      }
      for (int i = start; i < end; ++i)
         scratch.push_back(v[i]);
   }

   for (int i = start_a; i <= end_b; ++i)
      v[i] = scratch[i - start_a];

   return count;
}

size_t countInversions(vector<int> &v, int start, int end)
{
   if (start >= end)
      return 0;

   int start_a = start;
   int end_a = start + (end - start) / 2;
   int start_b = end_a + 1;
   int end_b = end;
   return countInversions(v, start_a, end_a) +
          countInversions(v, start_b, end_b) +
          mergeAndCountInversions(v, start_a, end_a, start_b, end_b);
}

size_t countInversions(vector<int> const &v)
{
   vector<int> copy = v; // we'll be counting via merge sort, so do not affect the input array
   return countInversions(copy, 0, v.size() - 1);
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

   // For each case, read the file and compute the number of inversions
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

      size_t inversions = countInversions(test_case);
      cout << "Inversions = " << inversions;
      cout << "; " << filename;
      cout << "; " << endl;
   }

   return 0;
}