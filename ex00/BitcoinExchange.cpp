#include "BitcoinExchange.hpp"
#include <cstdlib>
#include <fstream>
#include <limits>
#include <stdexcept>
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
  }
}
/*                                      */
static bool isValidDate(std::string &date) {
  if (date.length() != DATE_SIZE || date[date.length() - 1] != ' ')
    return (false);
  if (date[4] != '-' || date[7] != '-' || date[10] != ' ')
    return (false);
  if (date[0] == '-' || date[5] == '-' || date[8] == '-')
    return (false);
  char *rest;
  std::string year = date.substr(0, 4);
  std::string month = date.substr(5, 2);
  std::string day = date.substr(8, 2);
  if (month == "00" || day == "00")
    return (false);
  if (std::strtol(year.c_str(), &rest, 10) && *rest != '\0')
    return (false);
  if (std::strtol(month.c_str(), &rest, 10) > 12 && *rest != '\0')
    return (false);
  if (std::strtol(day.c_str(), &rest, 10) > 12 && *rest != '\0')
    return (false);
}
