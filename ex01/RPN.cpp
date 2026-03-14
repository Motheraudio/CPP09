#include "RPN.hpp"
static int PerformOperation(int a, int b, char op) {
  if (op == '+')
    return (a + b);
  if (op == '-')
    return (a - b);
  if (op == '/') {
    if (b == 0)
      throw(std::runtime_error("Error: Division by 0"));
    return (a / b);
  } else
    return (a * b);
}
static bool IsOperand(std::string c) {
  if (c == "+" || c == "-" || c == "/" || c == "*")
    return (true);
  return (false);
}

/*														*/
RPN::RPN() {}
RPN::~RPN() {}
int RPN::FetchResult(std::string expression) {
  try {
    std::stack<int> s;
    std::istringstream strm(expression);
    std::string token;
    int a;
    int b;
    int result;
    while (strm >> token) {
      if (IsOperand(token)) {
        if (s.size() < 2)
          throw(std::runtime_error("Error: Not enough ints"));
        b = s.top();
        s.pop();
        a = s.top();
        s.pop();
        result = PerformOperation(a, b, token[0]);
        s.push(result);
      } else if (token.length() > 1 || !isdigit(token[0]))
        throw(std::runtime_error("Error: Invalid token"));
      else
        s.push(token[0] - '0');
    }
    if (s.size() != 1)
      throw(std::runtime_error("Error: non-resolvable"));
    return (result);
  } catch (std::exception &e) {
    throw;
  }
}
