#include "BitcoinExchange.hpp"
#include <cstdlib>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <string.h>
BitcoinExchange::BitcoinExchange() {}

BitcoinExchange::BitcoinExchange(char *resolve) {
  try {
    this->_Cdb = createMap();
    this->_Cresolve = createMap(resolve);
  } catch (std::exception) {
    throw;
  }
}
BitcoinExchange::BitcoinExchange(const BitcoinExchange &other) {
  *this = other;
}

BitcoinExchange::~BitcoinExchange() {}

BitcoinExchange &BitcoinExchange::operator=(const BitcoinExchange &obj) {
  if (this == &obj)
    return (*this);
  this->_Cdb = obj._Cdb;
  this->_Cresolve = obj._Cresolve;
  return (*this);
}
/*                                      */
static bool hasValidSeparators(std::string date) {
  size_t spaces = 0;
  size_t minuses = 0;
  for (int i = 0; date[i]; i++) {
    if (date[i] == ' ') {
      spaces++;
      if (date[i + 1] != '\0')
        return (false);
    }
    if (date[i] == '-')
      minuses++;
  }
  if (spaces != 1 && minuses != 2)
    return (false);
  else
    return (true);
}

static bool isValidDate(const std::string &date) {
  if (!hasValidSeparators(date))
    return (false);
  char *copy = new char[date.length()];
  strcpy(copy, date.c_str());
  char *rest;
  int year = std::strtol(strtok(copy, "-"), &rest, 10);
  if (*rest != '\0')
    return (delete[] copy, false);
  int month = std::strtol(strtok(NULL, "-"), &rest, 10);
  if (*rest != '\0')
    return (delete[] copy, false);
  int day = std::strtol(strtok(NULL, " "), &rest, 10);
  if (*rest != '\0')
    return (delete[] copy, false);
  delete[] copy;
  // check for valid calendar dates
  return true;
}
/*														*/

std::map<std::string, std::string> BitcoinExchange::createMap() {
  std::ifstream db("data.csv");
  std::string key;
  std::string value;
  std::map<std::string, std::string> cdb;
  if (!db.is_open())
    throw(std::runtime_error("Database: Cannot open"));
  db.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  while (std::getline(db, key, ',') && std::getline(db, value, '\n'))
    cdb[key] = value;
  return (cdb);
}
std::map<std::string, std::string> BitcoinExchange::createMap(char *resolve) {
  std::ifstream rdb(resolve);
  std::string key;
  std::string value;
  std::map<std::string, std::string> crdb;
  if (!rdb.is_open())
    throw(std::runtime_error("Resolve File: Cannot open"));
  std::getline(rdb, key, '\n');
  if (key != "date | value")
    throw(std::runtime_error("Resolve File: Malformed header"));
  while (std::getline(rdb, key, '|') && std::getline(rdb, value, '\n'))
    crdb[key] = value;
  return (crdb);
}
void BitcoinExchange::printMap() {
  std::map<std::string, std::string>::iterator it;
  std::map<std::string, std::string>::iterator ite;
  it = this->_Cdb.begin();
  ite = this->_Cdb.end();
  std::cout << "date,exchange_rate" << std::endl;
  while (it != ite) {
    std::cout << it->first << "," << it->second << std::endl;
    it++;
  }
}
void BitcoinExchange::evaluateEntries() {
  // std::map<std::string, std::string>::iterator dbit;
  // std::map<std::string, std::string>::iterator dbite;
  // dbit = this->_Cdb.begin();
  // dbite = this->_Cdb.end();
  std::map<std::string, std::string>::iterator rit;
  std::map<std::string, std::string>::iterator rite;
  rit = this->_Cresolve.begin();
  rite = this->_Cresolve.end();
  while (rit != rite) {
    if (!isValidDate(rit->first))
      std::cout << "Error: not a valid separator -> " << rit->first
                << std::endl;
    rit++;
  }
}
