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
  try {

    PmergeMe::SortAll(args);
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  }
}
