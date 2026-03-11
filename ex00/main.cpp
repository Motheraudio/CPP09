#include "BitcoinExchange.hpp"
int main(int argc, char **argv) {
  (void)argc;
  BitcoinExchange exchange(argv[1]);
  exchange.printMap();
}
