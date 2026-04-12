#include "PmergeMe.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <sys/time.h>
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

// static void Printdq(std::deque<int> &v) {
//   std::deque<int>::iterator it = v.begin();
//   while (it != v.end()) {
//     std::cout << *it << " ";
//     it++;
//   }
//   std::cout << std::endl;
// }
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
}

static void SwapPairs(std::vector<int>::iterator it, int sig_mem) {
  for (int i = 0; i < sig_mem; i++)
    std::iter_swap(it - i, it + sig_mem - i);
}

static std::vector<int>::iterator
DetermineInsertionPos(std::vector<int>::iterator first, int value, int sig_mem,
                      int len) {
  while (len > 0) {
    int half = len / 2;
    std::vector<int>::iterator mid = first + half * sig_mem;
    if (AisBiggerOrEqualThanB(value, *mid)) {
      first = mid + sig_mem;
      len -= half + 1;
    } else
      len = half;
  }
  return first - (sig_mem - 1);
}

static void Insertion(std::vector<int> &main, std::vector<int> &pend,
                      int sig_mem) {
  if (pend.empty())
    return;
  int n_pend = pend.size() / sig_mem;
  if (n_pend == 0) { // just insert at end no pend blocks, lower rec will do
    while (!pend.empty()) {
      main.push_back(pend[0]);
      pend.erase(pend.begin());
    }
    return;
  }
  int n_paired = (int)main.size() / sig_mem - 2; // - a1 and b1, so -2
  if (n_paired < 0)
    n_paired = 0;
  std::vector<int> pair_pos(
      n_pend); // index in main seq of pair of k (the sig memeber!!)
  for (int k = 0; k < n_pend; k++) {
    if (k < n_paired)
      pair_pos[k] = (k + 2) * sig_mem + (sig_mem - 1); // search until here
    else
      pair_pos[k] = (int)main.size() - 1 + sig_mem; // search everything
  }
  std::vector<int> jacob = GenJacobsSec(n_pend);
  std::vector<int> order;
  std::vector<bool> done(n_pend, false);
  for (size_t i = 1; i < jacob.size(); i++) {
    int jval = std::min(jacob[i], n_pend + 1); // jacobsthal number or maximum
    int prev = jacob[i - 1];
    for (int y = jval; y > prev; y--) {
      int pb = y - 2; // b1 was inserted on seqgen
      if (pb >= 0 && pb < n_pend && !done[pb]) {
        order.push_back(pb);
        done[pb] = true;
      }
    }
    if (jval >= n_pend + 1)
      break;
  }
  for (int k = 0; k < n_pend; k++) {
    if (!done[k])
      order.push_back(k); // order not done? push at the end
  }
  for (size_t o = 0; o < order.size(); o++) {
    int idx = order[o];
    int pend_start = idx * sig_mem;
    int value = pend[pend_start + sig_mem - 1];
    int high_idx = pair_pos[idx];
    int low_idx = sig_mem - 1;
    int len = ((high_idx - low_idx) / sig_mem) + 1;
    std::vector<int>::iterator insertpos =
        DetermineInsertionPos(main.begin() + low_idx, value, sig_mem, len - 1);
    int insert_at = insertpos - main.begin(); // gives index
    main.insert(main.begin() + insert_at, pend.begin() + pend_start,
                pend.begin() + pend_start + sig_mem);
    for (int k = 0; k < n_pend; k++) {
      if (pair_pos[k] >= insert_at)
        pair_pos[k] += sig_mem;
    }
  }
  for (int k = n_pend - 1; k >= 0; k--)
    pend.erase(pend.begin() + k * sig_mem,
               pend.begin() + k * sig_mem + sig_mem);
  while (!pend.empty()) {
    main.push_back(pend[0]);
    pend.erase(pend.begin());
  }
}

static void RecSort(std::vector<int> &v, int sig_mem) {
  std::vector<int>::iterator it = v.begin() + (sig_mem - 1);
  unsigned int i = 0;
  while (i != v.size() / (2 * sig_mem) && it + sig_mem != v.end()) {
    if (AisBiggerThanB(*it, *(it + sig_mem)))
      SwapPairs(it, sig_mem);
    it += sig_mem * 2;
    i++;
  }
  if (i == 0)
    return;
  RecSort(v, sig_mem * 2);
  std::vector<int> main;
  std::vector<int> pend;
  CreateSeqs(v, pend, main, sig_mem);
  main.reserve(main.size() + pend.size());
  Insertion(main, pend, sig_mem);
  v = main;
  return;
}
/* DEQUE */
std::deque<int> GenJacobsSec_dq(int elements) {
  int curr = 1;
  std::deque<int> jacobs;
  jacobs.push_back(curr);
  for (int i = 1; i < elements; i++) {
    curr = (std::pow(2, i + 2) + std::pow(-1, i + 1)) / 3;
    jacobs.push_back(curr);
  }
  return jacobs;
}

static void CreateSeqs_dq(std::deque<int> &v, std::deque<int> &pend,
                          std::deque<int> &main, int sig_mem) {
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
}

static void SwapPairs_dq(std::deque<int>::iterator it, int sig_mem) {
  for (int i = 0; i < sig_mem; i++)
    std::iter_swap(it - i, it + sig_mem - i);
}

static std::deque<int>::iterator
DetermineInsertionPos_dq(std::deque<int>::iterator first, int value,
                         int sig_mem, int len) {
  while (len > 0) {
    int half = len / 2;
    std::deque<int>::iterator mid = first + half * sig_mem;
    if (AisBiggerOrEqualThanB(value, *mid)) {
      first = mid + sig_mem;
      len -= half + 1;
    } else
      len = half;
  }
  return first - (sig_mem - 1);
}

static void Insertion_dq(std::deque<int> &main, std::deque<int> &pend,
                         int sig_mem) {
  if (pend.empty())
    return;
  int n_pend = pend.size() / sig_mem;
  if (n_pend == 0) { // just insert at end no pend blocks, lower rec will do
    while (!pend.empty()) {
      main.push_back(pend[0]);
      pend.erase(pend.begin());
    }
    return;
  }
  int n_paired = (int)main.size() / sig_mem - 2; // - a1 and b1, so -2
  if (n_paired < 0)
    n_paired = 0;
  std::deque<int> pair_pos(
      n_pend); // index in main seq of pair of k (the sig memeber!!)
  for (int k = 0; k < n_pend; k++) {
    if (k < n_paired)
      pair_pos[k] = (k + 2) * sig_mem + (sig_mem - 1); // search until here
    else
      pair_pos[k] = (int)main.size() - 1 + sig_mem; // search everything
  }
  std::deque<int> jacob = GenJacobsSec_dq(n_pend);
  std::deque<int> order;
  std::deque<bool> done(n_pend, false);
  for (size_t i = 1; i < jacob.size(); i++) {
    int jval = std::min(jacob[i], n_pend + 1); // jacobsthal number or maximum
    int prev = jacob[i - 1];
    for (int y = jval; y > prev; y--) {
      int pb = y - 2; // b1 was inserted on seqgen
      if (pb >= 0 && pb < n_pend && !done[pb]) {
        order.push_back(pb);
        done[pb] = true;
      }
    }
    if (jval >= n_pend + 1)
      break;
  }
  for (int k = 0; k < n_pend; k++) {
    if (!done[k])
      order.push_back(k); // order not done? push at the end
  }
  for (size_t o = 0; o < order.size(); o++) {
    int idx = order[o];
    int pend_start = idx * sig_mem;
    int value = pend[pend_start + sig_mem - 1];
    int high_idx = pair_pos[idx];
    int low_idx = sig_mem - 1;
    int len = ((high_idx - low_idx) / sig_mem) + 1;
    std::deque<int>::iterator insertpos = DetermineInsertionPos_dq(
        main.begin() + low_idx, value, sig_mem, len - 1);
    int insert_at = insertpos - main.begin(); // gives index
    main.insert(main.begin() + insert_at, pend.begin() + pend_start,
                pend.begin() + pend_start + sig_mem);
    for (int k = 0; k < n_pend; k++) {
      if (pair_pos[k] >= insert_at)
        pair_pos[k] += sig_mem;
    }
  }
  for (int k = n_pend - 1; k >= 0; k--)
    pend.erase(pend.begin() + k * sig_mem,
               pend.begin() + k * sig_mem + sig_mem);
  while (!pend.empty()) {
    main.push_back(pend[0]);
    pend.erase(pend.begin());
  }
}

static void RecSort_dq(std::deque<int> &v, int sig_mem) {
  std::deque<int>::iterator it = v.begin() + (sig_mem - 1);
  unsigned int i = 0;
  while (i != v.size() / (2 * sig_mem) && it + sig_mem != v.end()) {
    if (AisBiggerThanB(*it, *(it + sig_mem)))
      SwapPairs_dq(it, sig_mem);
    it += sig_mem * 2;
    i++;
  }
  if (i == 0)
    return;
  RecSort_dq(v, sig_mem * 2);
  std::deque<int> main;
  std::deque<int> pend;
  CreateSeqs_dq(v, pend, main, sig_mem);
  Insertion_dq(main, pend, sig_mem);
  v = main;
  return;
}
void PmergeMe::SortAll(std::string args) {
  try {
    std::string token;
    std::vector<int> v;
    std::deque<int> dq;
    struct timeval t1;
    struct timeval t2;
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
    std::cout << "Before: ";
    PrintVec(v);
    // std::cout << "Before(DEQUE): ";
    // Printdq(dq);
    gettimeofday(&t1, NULL);
    uintmax_t timestart = t1.tv_sec * 1000 + t1.tv_usec;
    RecSort(v, 1);
    std::cout << "After: ";
    PrintVec(v);
    gettimeofday(&t1, NULL);
    uintmax_t timeend = t1.tv_sec * 1000 + t1.tv_usec;
    std::cout << "Time to process " << v.size()
              << " elements with std::vector: " << timeend - timestart
              << " microseconds" << std::endl;
    gettimeofday(&t2, NULL);
    timestart = t2.tv_sec * 1000 + t2.tv_usec;
    RecSort_dq(dq, 1);
    // std::cout << "After(DEQUE): ";
    // Printdq(dq);
    gettimeofday(&t2, NULL);
    timeend = t2.tv_sec * 1000 + t2.tv_usec;
    std::cout << "Time to process " << v.size()
              << " elements with std::deque: " << timeend - timestart
              << " microseconds" << std::endl;
  } catch (std::exception &e) {
    throw;
  }
  // std::cout << g_comparisons / 2 << std::endl;
  //  PrintVec(v);
  //  Printdq(dq);
}
