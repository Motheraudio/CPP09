#pragma once
#include <deque>
#include <string>
#include <vector>
class PmergeMe {
public:
  static void SortAll(std::string args);

private:
  PmergeMe();
  ~PmergeMe();
  PmergeMe(const PmergeMe &obj);
  PmergeMe &operator=(const PmergeMe &obj);
  static void SortV(std::vector<int> V);
};
