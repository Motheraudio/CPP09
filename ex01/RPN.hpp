#pragma once
#include <iostream>
#include <sstream>
#include <stack>
class RPN {
public:
  static int FetchResult(std::string expression);

private:
  RPN();
  ~RPN();
  RPN(const RPN &other);
  RPN &operator=(const RPN &other);
};
