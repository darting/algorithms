#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <stdexcept>

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

using namespace std;

enum class NodeType {
  number,
  symbol,
  list
};

struct Node {
  NodeType type;
};

struct Number : Node {
  double value;
  Number(const double &v) : value(v) {
    type = NodeType::number;
  }
};

struct Symbol : Node {
  string value;
  Symbol(const string &s) : value(s) {
    type = NodeType::symbol;
  }
};

struct List : Node {
  vector<shared_ptr<Node>> children;
  List() {
    type = NodeType::list;
  }
};


queue<string> tokenize(string program) {
  queue<string> tokens;
  auto it = program.begin();
  while (it != program.end()) {
    auto c = *it;
    if (c == '(' || c == ')') {
      tokens.push(string{c});
    } else if (c != ' ' && c != '\n') {
      auto end = it;
      while (*end != ' ' &&
             *end != '(' &&
             *end != ')') {
        ++end;
      }
      tokens.push(string(it, end));
      it = end;
      continue;
    }
    ++it;
  }
  return tokens;
}

shared_ptr<Node> atom(const string &token) {
  try {
    return make_shared<Number>(Number(stod(token)));
  } catch (const invalid_argument &ex) {
    return make_shared<Symbol>(Symbol(token));
  }
}

shared_ptr<Node> parse(queue<string> &tokens) {
  auto token = tokens.front();
  tokens.pop();
  if (token == "(") {
    List l;
    while (tokens.front() != ")") {
      l.children.push_back(parse(tokens));
    }
    if (tokens.empty()) {
      throw "syntax error, EOF while reading";
    }
    tokens.pop();
    return make_shared<List>(l);
  } else if (token == ")") {
    throw "syntax error, unexpected ')'";
  } else {
    return atom(token);
  }
}


void printTree(const shared_ptr<Node> &node, const int &deep) {
  for (int i = 0; i < deep; ++i) {
    cout << " ";
  }
  if (node->type == NodeType::list) {
    auto list = static_cast<const List*>(node.get());
    if (list->children.size() > 0) {
      cout << "symbol: " << static_cast<const Symbol*>(list->children[0].get())->value << endl;
      for (size_t i = 1; i < list->children.size(); ++i) {
        printTree(list->children[i], deep + 1);
      }
    } else {
      cout << "list: Nil" << endl;
    }
  } else if (node->type == NodeType::number) {
    auto number = static_cast<const Number*>(node.get());
    cout << "number: " << number->value << endl;
  } else if (node->type == NodeType::symbol) {
    auto symbol = static_cast<const Symbol*>(node.get());
    cout << "symbol: " << symbol->value << endl;
  }
}




string program1 = R"(
    (if (> (val x) 0)
       (fn (+ (aref A i) 1)
        (quote (one two))))
    )";


TEST_CASE("tokenize") {

  auto tokens = tokenize(program1);

  vector<string> expect { "(", "if", "(", ">", "(", "val", "x", ")", "0", ")", "(", "fn", "(", "+", 
                          "(", "aref", "A", "i", ")", "1", ")", "(", "quote", "(", "one", "two", ")", 
                          ")", ")", ")" };
  for (auto s : expect) {
    REQUIRE(tokens.front() == s);
    tokens.pop();
  }
}

TEST_CASE("parse") {

  auto tokens = tokenize(program1);
  auto tree = parse(tokens);

  printTree(tree, 0);
}
























