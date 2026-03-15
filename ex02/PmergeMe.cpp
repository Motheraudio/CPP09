#include "PmergeMe.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
int g_comparisons = 0;

PmergeMe::PmergeMe() {}

PmergeMe::~PmergeMe() {}

static bool AisBiggerThanB(int a, int b) {
  g_comparisons++;
  return (a > b);
}

static void PrintVec(std::vector<int> &v) {
  std::vector<int>::iterator it = v.begin();
  while (it != v.end()) {
    std::cout << *it << " ";
    it++;
  }
  std::cout << std::endl;
}

// sig_mem: offset to the significant member of a pair to compare for recursion
// call with sig_mem=1 first time

static void SwapPairs(std::vector<int>::iterator it, int sig_mem) {
  for (int i = 0; i < sig_mem; i++)
    std::iter_swap(it - i, it + sig_mem - i);
}
static void RecSort(std::vector<int> &v, int sig_mem) {
  std::vector<int>::iterator it = v.begin() + (sig_mem - 1);
  std::vector<int> small;
  unsigned int i = 0;
  std::cout << "BEFORE, with sig_mem = " << sig_mem << std::endl;
  while ((i * sig_mem) + sig_mem < v.size() && it + sig_mem != v.end() &&
         i != v.size() / (2 * sig_mem)) {
    if (AisBiggerThanB(*it, *(it + sig_mem)))
      SwapPairs(it, sig_mem);
    PrintVec(v);
    it += sig_mem * 2;
    i++;
  }
  std::cout << "AFTER, with sig_mem = " << sig_mem << std::endl;
  PrintVec(v);
  if (i == 0)
    return;
  RecSort(v, sig_mem * 2);
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
  // Sortdq
  // print
}
