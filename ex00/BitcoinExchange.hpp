#pragma once

#include <iostream>
#include <map>
#define DATE_SIZE 11
class BitcoinExchange {
public:
  BitcoinExchange(char *resolve);
  BitcoinExchange(const BitcoinExchange &obj);
  ~BitcoinExchange();
  BitcoinExchange &operator=(const BitcoinExchange &obj);

  void printMap();
  void evaluateEntries();

private:
  BitcoinExchange();

  std::map<std::string, std::string> _Cdb;
  std::map<std::string, std::string> _Cresolve;
  std::map<std::string, std::string> createMap();
  std::map<std::string, std::string> createMap(char *resolve);
};
