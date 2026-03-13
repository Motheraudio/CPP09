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

static bool isCalendarAccurate(int year, int month, int day) {
  // std::cout << year << "-" << month << "-" << day << std::endl;
  if (year < 2009 || month <= 0 || month > 12 || day <= 0 || day > 31)
    return (false);
  if (year == 2009 && month == 1 && day == 1)
    return (false);
  if (month == 2 && day == 29) {
    if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
      return (true);
    else
      return (false);
  }
  if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30)
    return (false);
  return (true);
}

static bool isValidDate(const std::string &date) {
  if (!hasValidSeparators(date))
    return (false);
  char *copy = new char[date.length() + 1];
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
  if (isCalendarAccurate(year, month, day))
    return true;
  return false;
}

static std::string isValidNumber(std::string input) {
  if (input.empty() || input[0] != ' ')
    return ("Error: Bad input ->" + input);
  char *copy = new char[input.length() + 1];
  strcpy(copy, input.c_str());
  char *end;
  double number = std::strtod(&copy[1], &end);
  if (*end != '\0')
    return (delete[] copy, "Error: Bad input ->" + input);
  delete[] copy;
  if (number < 0.0)
    return ("Error: Not a positive number");
  if (number > 1000.00)
    return ("Error: Number too high");
  return (input);
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
  std::string out;
  while (rit != rite) {
    if (!isValidDate(rit->first)) {
      std::cout << "Error: Bad input -> " << rit->first << std::endl;
      rit++;
      continue;
    }
    out = isValidNumber(rit->second);
    if (out != rit->second)
      std::cout << out << std::endl;
    else {
      std::map<std::string, std::string>::iterator locate =
          this->_Cdb.lower_bound(rit->first);
      if (strtol(rit->first.substr(0, rit->first.find("-")).c_str(), NULL, 10) >
          2022)
        std::cout << rit->first << "-> "
                  << strtod(this->_Cdb.rbegin()->second.c_str(), NULL) *
                         strtod(&rit->second.c_str()[1], NULL)
                  << std::endl;
      else if (locate->first == rit->first)
        std::cout << rit->first << "-> "
                  << strtod(locate->second.c_str(), NULL) *
                         strtod(&rit->second.c_str()[1], NULL)
                  << std::endl;
      else if (locate == _Cdb.begin())
        std::cout << "Error: Bad input -> " << rit->first << std::endl;
      else {
        locate--;
        std::cout << rit->first << "-> "
                  << strtod(locate->second.c_str(), NULL) *
                         strtod(&rit->second.c_str()[1], NULL)
                  << std::endl;
      }
    }

    rit++;
  }
}
