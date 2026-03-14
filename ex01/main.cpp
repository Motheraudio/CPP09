#include "RPN.hpp"
int main(int argc, char **argv) {
  if (argc != 2)
    std::cerr << "use: ./RPN \"Expression\"" << std::endl;
  try {
    std::cout << RPN::FetchResult(argv[1]) << std::endl;
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
}
