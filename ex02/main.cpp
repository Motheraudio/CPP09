#include "PmergeMe.hpp"
#include "iostream"
int main(int argc, char **argv) {
  if (argc == 1) {
    std::cerr << "usage: ./PmergeMe [ints]" << std::endl;
    return 1;
  }
  std::string args;
  for (int i = 1; argv[i]; i++) {
    args += argv[i];
    args += " ";
  }
  std::cout << args << std::endl;
  PmergeMe::SortAll(args);
}
