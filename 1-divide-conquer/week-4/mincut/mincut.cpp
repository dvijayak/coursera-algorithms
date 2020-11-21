/**
 * Your task is to code up and run the randomized contraction algorithm for the min cut problem and use it on the given graph to compute the min cut.
 * (HINT: Note that you'll have to figure out an implementation of edge contractions. Initially, you might want to do this naively, creating a new graph from the old every time there's an edge contraction. But you should also think about more efficient implementations.)
 * (WARNING: As per the video lectures, please make sure to run the algorithm many times with different random seeds, and remember the smallest cut that you ever find.)
 */

#include <iostream>
#include <fstream>
#include <dirent.h> // UNIX only; for Windows, need to get it elsewhere, see https://stackoverflow.com/a/612176/3477043
#include <string>
#include <regex>

#include <limits>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include <sstream>
#include <algorithm>

using namespace std;

#pragma region Utilities
template <typename T, typename U>
ostream &operator<<(ostream &os, pair<T, U> const &p)
{
   os << "(" << p.first << "," << p.second << ")";
   return os;
}

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

// Courtesy of https://stackoverflow.com/a/9437426
std::vector<std::string> split(const string& input, const string& regex) {
    // passing -1 as the submatch index parameter performs splitting
    std::regex re(regex);
    std::sregex_token_iterator
        first{input.begin(), input.end(), re, -1},
        last;
    return {first, last};
}
#pragma endregion

// An undirected graph
class Graph
{
public:
   struct Node;
   typedef pair<Node*, Node*> Edge; // this is an UNORDERED pair

   struct Node
   {
      int value;
      vector<Edge> edges;

      Node(int _value) : value(_value) {}
   };

private:
   friend ostream& operator<<(ostream &os, Edge const &e)
   {
      os << "(" << e.first->value << "," << e.second->value << ")";
      return os;
   }
   friend ostream& operator<<(ostream &os, Node const &n)
   {
      os << n.value;
      return os;
   }
   friend ostream& operator<<(ostream &os, Node* n)
   {
      os << n->value;
      return os;
   }

   struct EdgeHash
   {
      size_t operator()(Edge const& edge) const {

         // Mainly for demonstration purposes, i.e. works but is overly simple
         // In the real world, use sth. like boost.hash_combine
         return hash<Node*>()(edge.first) ^ hash<Node*>()(edge.second);
      }
   };

   struct EdgeComparator
   {
      bool operator()(Edge const& A, Edge const& B) const
      {
         // If both elements of A are in B, they are considered the same
         return (A.first == B.first && A.second == B.second || A.first == B.second && A.second == B.first);
      }
   };
   typedef vector<Edge> Edges;

public:
   typedef vector<unique_ptr<Node>> Nodes;
   Nodes nodes;
   Edges edges; // responsible for ownership of edges

private:

   Node* GetOtherNode(Edge const& edge, Node* node)
   {
      if (node == nullptr)
         return nullptr;

      if (edge.first == node)
         return edge.second;
      if (edge.second == node)
         return edge.first;

      return nullptr;
   }

   unordered_set<Edge, EdgeHash, EdgeComparator> edges_cut;
   void CutEdge(Edge const& edge)
   {
      edges_cut.insert(edge);
   }

   vector<Edge> GetUncutEdges(Node const& node) const
   {
      vector<Edge> result;
      for (auto it = node.edges.begin(); it != node.edges.end(); ++it)
      {
         if (edges_cut.find(*it) == edges_cut.end())
            result.push_back(*it);
      }
      return move(result);
   }

   Edge ChooseRandomUncutEdge() const
   {
      // Inefficient but w/e it works
      vector<Edge> ram_copy;
      copy_if(
         edges.begin(),
         edges.end(),
         back_inserter(ram_copy),
         [&](auto const& edge) { return edges_cut.find(edge) == edges_cut.end(); }
      );

      // Pick a random edge from the above list of uncut edges
      random_device rd;
      default_random_engine generator(rd());
      uniform_int_distribution<int> get_random_index(0, ram_copy.size() - 1);
      int i = get_random_index(generator);
      return move(ram_copy[i]);
   }

   bool EdgesAreParallel(Edge const& A, Edge const& B)
   {
      return A == B;
   }

public:
   Node* MakeNode(int value)
   {
      Node* p_node = new Node(value);
      nodes.push_back(unique_ptr<Node>(p_node));
      return p_node;
   }

   void MakeEdge(Node* A, Node* B)
   {
      if (A == nullptr || B == nullptr)
         return;
      
      Edge edge{A, B};
      edges.push_back(edge);
      A->edges.push_back(edge);
      B->edges.push_back(edge);
   }

   int ComputeMinCut()
   {
      // Initialize relevant data structures
      int count = nodes.size();
      

      // We are done once we have only two nodes left to consider
      for (; count > 2; --count) // by the definition of the algorithm, every iteration results in 1 less node to be considered
      {
         Edge edge_chosen = ChooseRandomUncutEdge();

         // Cut it and all its parallel edges 
         vector<Edge> to_cut{edge_chosen};
         copy_if(
            edges.begin(),
            edges.end(),
            back_inserter(to_cut),
            [&](auto const& edge) { return edges_cut.find(edge) == edges_cut.end() && EdgesAreParallel(edge_chosen, edge); }
         );
         for (auto const& x : to_cut)
            CutEdge(x);

         // Find all uncut edges that connect to the chosen edge's nodes
         auto candidates1 = GetUncutEdges(*edge_chosen.first);
         auto candidates2 = GetUncutEdges(*edge_chosen.second);

         // Make a new super node that connects to these edges
         Node* p_super_node = MakeNode(0); // actual value doesn't matter
         for (auto const& candidate : candidates1)
         {
            CutEdge(candidate);
            MakeEdge(p_super_node, GetOtherNode(candidate, edge_chosen.first));
         }
         for (auto const& candidate : candidates2)
         {
            CutEdge(candidate);
            MakeEdge(p_super_node, GetOtherNode(candidate, edge_chosen.second));
         }
      }

      // The result is the total number of remaining uncut edges
      int result = 0;
      for (auto const& edge : edges)
         if (edges_cut.find(edge) == edges_cut.end())
            result++;
      return result;
   }
};



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

   int const n_start = 1, n = test_case_files.size();
   for (auto it = test_case_files.begin() + n_start - 1; it != test_case_files.begin() + n_start - 1 + n; ++it)
   {
      string const &filename = *it;
      string const filepath = TESTDIR + "/" + filename;
      ifstream file(filepath);

      vector<string> lines;
      string line;
      while (getline(file, line))
         lines.push_back(line);

      cout << filename << ":" << endl;

      vector<vector<int>> tokens;
      for (auto const& line : lines)
      {
         vector<int> actual_result;
         auto result = split(line, "\\s+");
         for (auto &s : result)
         {
            stringstream ss;
            ss << s;
            int d;
            ss >> d;
            actual_result.push_back(d);
         }
         tokens.push_back(actual_result);
      }

      // Do this once and for all, so we don't have to keep repeating this process
      unordered_set<int> nodes_to_make;
      for (auto const& row : tokens)
         for (auto v : row)
            nodes_to_make.insert(v);

      vector<pair<int, int>> edges_to_make;
      for (auto const& row : tokens)
      {
         int first = row[0];
         for (int i = 1; i < row.size(); ++i)
         {
            pair<int, int> edge = {first, row[i]};
            
            // Super inefficient but w/e
            bool found = false;
            for (auto const& existing : edges_to_make)
            {
               found = existing.first == edge.first && existing.second == edge.second || existing.first == edge.second && existing.second == edge.first;
               if (found)
                  break;
            }

            if (!found)
               edges_to_make.push_back(move(edge));
         }
      }

      // Perform N repetitions of the min cut algorithm, in a terribly inefficient way
      int n = lines.size();
      int min = numeric_limits<int>::max();
      for (int i = 0; i < n; ++i)
      {
         Graph graph;

         // be efficient
         unordered_map<int, Graph::Node*> nodes_cache;
         for (auto x : nodes_to_make)
         {
            auto p = graph.MakeNode(x);
            nodes_cache.insert(make_pair(x, p));
         }
         for (auto const& edge : edges_to_make)
         {
            auto A = nodes_cache.find(edge.first)->second;
            auto B = nodes_cache.find(edge.second)->second;
            graph.MakeEdge(A, B);
         }
         
         int cut = graph.ComputeMinCut();
         if (cut < min)
            min = cut;
      }
      cout << endl;
      cout << "Min cut is " << min << endl;
      cout << "; " << endl;
   }

   return 0;
}