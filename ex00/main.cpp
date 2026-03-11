#include "BitcoinExchange.hpp"
int main(int argc, char **argv) {
  if (argc != 2)
    std::cout << "Error: Could not open file" << std::endl;
  try {
    BitcoinExchange exchange(argv[1]);
    exchange.printMap();
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  }
}
