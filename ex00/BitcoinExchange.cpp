#include "BitcoinExchange.hpp"
#include <cstdlib>
#include <fstream>
#include <limits>
#include <stdexcept>
BitcoinExchange::BitcoinExchange() {}

BitcoinExchange::BitcoinExchange(char *resolve) { this->_Cdb = createMap(); }
BitcoinExchange::BitcoinExchange(const BitcoinExchange &other) {}

BitcoinExchange::~BitcoinExchange() {}

// BitcoinExchange &BitcoinExchange::operator=(const BitcoinExchange &obj) {}
/*														*/

std::map<std::string, double> BitcoinExchange::createMap() {
  std::ifstream db("data.csv");
  std::string key;
  std::string value;
  char *end;
  std::map<std::string, double> cdb;
  if (!db.is_open())
    throw(std::runtime_error("Database: Cannot open"));
  db.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  while (!db.eof()) {
    std::getline(db, key, ',');
    std::getline(db, value, '\n');
    cdb[key] = std::strtod(value.c_str(), &end);
    if (*end != '\0')
      throw(std::runtime_error("Database: a value is not correctly formatted"));
  }
  return (cdb);
}
// std::map<std::string, double> BitcoinExchange::createMap(char *resolve) {}
void BitcoinExchange::printMap() {
  std::map<std::string, double>::iterator it;
  std::map<std::string, double>::iterator ite;
  it = this->_Cdb.begin();
  ite = this->_Cdb.end();
  std::cout << "date,exchange_rate" << std::endl;
  while (it != ite) {

    std::cout << it->first << "," << it->second << std::endl;
    it++;
  }
}
