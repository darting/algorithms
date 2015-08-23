#include <iostream>
#include <stack>

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

using namespace std;

bool is_operator(const char &c) {
  return c == '+' ||
         c == '-' ||
         c == '*' ||
         c == '/' ||
         c == '(' ||
         c == ')';
}

int precedence(const char &c) {
  if (c == '+' || c == '-') {
    return 1;
  } else if (c == '*' || c == '/') {
    return 2;
  } else if (c == '(' || c == ')') {
    return 3;
  }
  return 0;
}

double perform(const char &c, const double &a, const double &b) {
  if (c == '+') {
    return a + b;
  } else if (c == '-') {
    return a - b;
  } else if (c == '*') {
    return a * b;
  } else if (c == '/') {
    return a / b;
  }
  return 0.0;
}

// postfix
// 4 1 * 5 + 6 1 * +
// => 15
int evaluate_postfix(const string & expression) {
  stack<double> s;
  size_t i = 0;
  for (size_t j = 0; j < expression.size(); ++j) {
    char c = expression[j];
    if (c == ' ') {
      if (i < j) {
        s.push(stod(expression.substr(i, j - i)));
      }
      i = j + 1;
    } else if (is_operator(c)) {
      ++i;
      double a = s.top();
      s.pop();
      double b = s.top();
      s.pop();
      s.push(perform(c, a, b));
    }
  }
  return s.top();
}

// a + b * c + (d * e + f) * g
// a b c * + d e * f + g * +
//
// 1 + 2 * 3 + ( 4 * 5 + 6 ) * 7 = 189
// 1 2 3 * + 4 5 * 6 + 7 * +
string infix_postfix(const string &exp) {
  stack<char> op;
  string result;
  for (size_t i = 0; i < exp.size(); ++i) {
    char c = exp[i];
    if (is_operator(c)) {
      if (op.empty() || c == '(') {
        op.push(c);
      } else if (c == ')') {
        while(op.top() != '(') {
          result += op.top();
          result += ' ';
          op.pop();
        }
        op.pop();
      } else {
        int curr = precedence(c);
        while(!op.empty()) {
          if (op.top() == '(') {
            break;
          }
          int prev = precedence(op.top());
          if (curr <= prev) {
            result += op.top();
            result += ' ';
            op.pop();
          } else {
            break;
          }
        }
        op.push(c);
      }
    } else if (c != ' '){
      result += c;
      result += ' ';
    }
  }
  while (!op.empty()) {
    result += op.top();
    result += ' ';
    op.pop();
  }
  return result;
}

TEST_CASE( "evaludate postfix" ) {
  REQUIRE(evaluate_postfix("4 1 * 5 + 6 1 * +") == 15);
}

TEST_CASE( "infix to postfix" ) {
  auto exp = infix_postfix("1 + 2 * 3 + ( 4 * 5 + 6 ) * 7");
  auto val = evaluate_postfix(exp);
  REQUIRE(exp == "1 2 3 * + 4 5 * 6 + 7 * + ");
  REQUIRE(val == 189);
}
