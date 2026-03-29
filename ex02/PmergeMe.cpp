#include "PmergeMe.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>
int g_comparisons = 0;

PmergeMe::PmergeMe() {}

PmergeMe::~PmergeMe() {}

static bool AisBiggerThanB(int a, const int b) {
  g_comparisons++;
  return (a > b);
}
static bool AisBiggerOrEqualThanB(int a, const int b) {
  g_comparisons++;
  return (a >= b);
}
/******************************/
std::vector<int> GenJacobsSec(int elements) {
  int curr = 1;
  std::vector<int> jacobs;
  jacobs.push_back(curr);
  for (int i = 1; i < elements; i++) {
    curr = (std::pow(2, i + 2) + std::pow(-1, i + 1)) / 3;
    jacobs.push_back(curr);
  }
  return jacobs;
}
/*******************************/

static void PrintVec(std::vector<int> &v) {
  std::vector<int>::iterator it = v.begin();
  while (it != v.end()) {
    std::cout << *it << " ";
    it++;
  }
  std::cout << std::endl;
}

static void CreateSeqs(std::vector<int> &v, std::vector<int> &pend,
                       std::vector<int> &main, int sig_mem) {
  unsigned int leftover = v.size() % (sig_mem * 2);
  // insert b1 and a1 into main
  for (int i = 0; i < sig_mem * 2; i++) {
    main.push_back(v[i]);
  }
  unsigned int i = sig_mem * 2;
  int y = 0;
  while (i < (v.size() - leftover)) {
    while (y < sig_mem) {
      pend.push_back(v[i + y]);
      y++;
    }
    y = 0;
    i += sig_mem;
    while (y < sig_mem) {
      main.push_back(v[i + y]);
      y++;
    }
    y = 0;
    i += sig_mem;
  }
  while (i < v.size()) {
    pend.push_back(v[i]);
    i++;
  }
  // std::cout << "MAIN: ";
  // PrintVec(main);
  // std::cout << "PEND: ";
  // PrintVec(pend);
}
// sig_mem: offset to the significant member of a pair to compare for recursion
// call with sig_mem=1 first time

static void SwapPairs(std::vector<int>::iterator it, int sig_mem) {
  for (int i = 0; i < sig_mem; i++)
    std::iter_swap(it - i, it + sig_mem - i);
}
// static std::vector<int>::iterator BSearch(std::vector<int>::iterator first,
//                                           std::vector<int>::iterator last,
//                                           int value, int sig_mem, int len) {
//   std::vector<int>::iterator low = first + (sig_mem - 1);
//   std::vector<int>::iterator high = last + (sig_mem - 1);
//   std::vector<int>::iterator mid = low + (len / 2 + sig_mem - 1);
//   for (int i = 0; i < len; i++) {
//     if (AisBiggerOrEqualThanB(*mid, value))
// 			{
//
// 			}
//   }
// }
// 7 8 9    11
//
static std::vector<int>::iterator
DetermineInsertionPos(std::vector<int>::iterator first,
                      std::vector<int>::iterator last, int value, int sig_mem,
                      int len) {
  while (len > 0) {
    int half = len / 2;
    std::vector<int>::iterator mid = first + half * sig_mem;
    if (AisBiggerOrEqualThanB(value, *mid)) {
      first = mid;
      len -= half + 1;
    } else
      len = half;
  }
  return first + 1; // to return where to emplace stuff
}
// 1 2  3 4  5 6        3 3
static void Insertion(std::vector<int> &main, std::vector<int> &pend,
                      int sig_mem) {
  if (pend.empty())
    return;
  int leftovers = pend.size() % sig_mem;
  std::vector<int> insertionmap = GenJacobsSec(pend.size() / sig_mem);
  std::vector<int>::iterator low = main.begin() + (sig_mem - 1);
  for (int i = 1; i < insertionmap.size(); i++) {
    std::vector<int>::iterator high = low + (insertionmap[i] - 1) * sig_mem;
    std::vector<int>::iterator saved_high = high;
    for (int y = insertionmap[i]; y != insertionmap[i - 1]; y--) {
      std::vector<int>::iterator insertpos = DetermineInsertionPos(
          low, high, *pend.begin(), sig_mem, pend.size() / sig_mem);
      main.insert(insertpos, pend.begin(), pend.begin() + sig_mem - 1);
      std::cout << y << ": " << *insertpos << std::endl;
      pend.erase(pend.begin(), pend.begin() + sig_mem - 1);
      high -= sig_mem;
    }
    low = saved_high;
  }
  while (leftovers != 0) {
    std::vector<int>::iterator insertpos_l = DetermineInsertionPos(
        main.begin(), main.end(), *pend.begin(), 1, pend.size());
    main.insert(insertpos_l, *pend.begin());
    pend.erase(pend.begin());
    leftovers--;
  }
  // std::vector<int>::iterator high = main.begin() + 2 * sig_mem;
  // SKIP index 0, already inserted on creation
}
static void RecSort(std::vector<int> &v, int sig_mem) {
  std::vector<int>::iterator it = v.begin() + (sig_mem - 1);
  unsigned int i = 0;
  // std::cout << "BEFORE, with sig_mem = " << sig_mem << std::endl;
  while (i != v.size() / (2 * sig_mem) && it + sig_mem != v.end()) {
    if (AisBiggerThanB(*it, *(it + sig_mem)))
      SwapPairs(it, sig_mem);
    // PrintVec(v);
    it += sig_mem * 2;
    i++;
  }
  // PrintVec(v);
  // std::cout << "AFTER, with sig_mem = " << sig_mem << std::endl;
  // PrintVec(v);
  if (i == 0)
    return;
  // PrintVec(pend);
  // PrintVec(main);
  // if (!pend.empty())
  RecSort(v, sig_mem * 2); // or the v instead of main? we'll see
  std::vector<int> main;
  std::vector<int> pend;
  CreateSeqs(v, pend, main, sig_mem);
  Insertion(main, pend, sig_mem);
  v = main; // i actually need to insert the carry on
  // std::cout << g_comparisons << std::endl;
  // PrintVec(v);
  return;
}
void PmergeMe::SortAll(std::string args) {
  std::string token;
  std::vector<int> v;
  std::deque<int> dq;
  int curr;
  char *rest;
  std::istringstream stream(args);
  while (stream >> token) {
    curr = strtol(token.c_str(), &rest, 10);
    if (*rest != '\0')
      throw(std::runtime_error("Error: wrong input"));
    v.push_back(curr);
    dq.push_back(curr);
  }
  RecSort(v, 1);
  std::cout << g_comparisons << std::endl;
  PrintVec(v);
  // Sortdq
  // print
}
