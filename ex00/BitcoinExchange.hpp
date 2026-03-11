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

private:
  std::map<std::string, float> _Cdb;
  std::map<std::string, float> _Cresolve;
  std::map<std::string, float> createMap();
  std::map<std::string, float> createMap(char *resolve);
};

std::ostream &operator<<(std::ostream &os, const BitcoinExchange &obj);
