#include <iostream>
#include <sstream>
#include <stack>
class RPN {
public:
  RPN();
  ~RPN();
  static int FetchResult(std::string expression);

private:
  RPN(const RPN &other);
  RPN &operator=(const RPN &other);
};
