#pragma once

#include <iostream>
#include <map>
class BitcoinExchange {
public:
  BitcoinExchange();
  BitcoinExchange(char *resolve);
  BitcoinExchange(const BitcoinExchange &obj);
  ~BitcoinExchange();

  BitcoinExchange &operator=(const BitcoinExchange &obj);
  void printMap();

private:
  std::map<std::string, double> _Cdb;
  std::map<std::string, double> _Cresolve;
  std::map<std::string, double> createMap();
  std::map<std::string, double> createMap(char *resolve);
};

std::ostream &operator<<(std::ostream &os, const BitcoinExchange &obj);
