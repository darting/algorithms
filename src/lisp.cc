#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <functional>

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

using namespace std;

enum class NodeType {
  number,
  symbol,
  list,
  boolean,
  procedure,
  func
};

class Node {
public:
  typedef shared_ptr<Node> Ptr;
  NodeType type;
};

class Number : public Node {
public:
  double value;
  Number(const double &v) : value(v) {
    type = NodeType::number;
  }
};

class Symbol : public Node {
public:
  string value;
  Symbol(const string &s) : value(s) {
    type = NodeType::symbol;
  }
};

class List : public Node {
public:
  vector<shared_ptr<Node>> children;
  List() {
    type = NodeType::list;
  }
};

class Boolean : public Node {
public:
  Boolean() {
    type = NodeType::boolean;
  }
public:
  static const Node::Ptr True;
  static const Node::Ptr False;
};

const Node::Ptr Boolean::True = make_shared<Boolean>();
const Node::Ptr Boolean::False = make_shared<Boolean>();

class Func : public Node {
public:
  Func() {
    type = NodeType::func;
  }
  virtual ~Func() {}
  virtual Node::Ptr call(vector<Node::Ptr> args) = 0;
};

// class Op : public Node {
// public:
//   typedef function<Node::Ptr(const vector<Node::Ptr>&)> FuncType;
// private:
//   FuncType func_;
// public:
//   Op(FuncType f) : func_(f) {}
//   Node::Ptr call(const vector<Node::Ptr> &args) {
//     return func_(args);
//   }
// };

// template<typename T>
// Node::Ptr Add1(const vector<Node::Ptr> &args) {
//   return nullptr;
// }

// template<typename T>
// Node::Ptr Add1(const vector<Node::Ptr> &args) {
//   return nullptr;
// }

class Add : public Func {
public:
  Node::Ptr call(vector<Node::Ptr> args) override {
    auto l = static_cast<const Number*>(args[0].get())->value;
    auto r = static_cast<const Number*>(args[1].get())->value;
    return make_shared<Number>(l + r);
  }
};

class Subtract : public Func {
public:
  Node::Ptr call(vector<Node::Ptr> args) override {
    auto l = static_cast<const Number*>(args[0].get())->value;
    auto r = static_cast<const Number*>(args[1].get())->value;
    return make_shared<Number>(l - r);
  }
};

class Multiply : public Func {
public:
  Node::Ptr call(vector<Node::Ptr> args) override {
    auto l = static_cast<const Number*>(args[0].get())->value;
    auto r = static_cast<const Number*>(args[1].get())->value;
    return make_shared<Number>(l * r);
  }
};

class Divide : public Func {
public:
  Node::Ptr call(vector<Node::Ptr> args) override {
    auto l = static_cast<const Number*>(args[0].get())->value;
    auto r = static_cast<const Number*>(args[1].get())->value;
    return make_shared<Number>(l / r);
  }
};

class Env {
public:
  typedef shared_ptr<Env> Ptr;
  typedef string KeyType;
  typedef Node::Ptr ValueType;
  typedef unordered_map<KeyType, ValueType> ValuesType;
private:
  Ptr outer_;
  ValuesType values_;

public:
  Env(Ptr outer = nullptr) : outer_(outer) {}
  
  ValueType Find(const KeyType &key) const {
    auto it = values_.find(key);
    if (it == values_.end()) {
      if (outer_) {
        return outer_->Find(key);
      } else {
        return nullptr;
      }
    }
    return it->second;
  }
  
  bool Emplace(const KeyType &key, ValueType node) {
    return values_.emplace(key, node).second;
  }
};

Env::Ptr standardEnv() {
  Env::Ptr env = make_shared<Env>();
  env->Emplace("+", make_shared<Add>());
  env->Emplace("-", make_shared<Subtract>());
  env->Emplace("*", make_shared<Multiply>());
  env->Emplace("/", make_shared<Divide>());
  return env;
}


Node::Ptr eval(Node::Ptr x, Env::Ptr env);

class Procedure : public Node {
private:
  vector<Node::Ptr> parms_;
  Node::Ptr body_;
  Env::Ptr env_;
public:
  Procedure(const vector<Node::Ptr> &parms, Node::Ptr body, Env::Ptr env)
  : parms_(parms),
    body_(body),
    env_(env) {
      type = NodeType::procedure;
    }
  Node::Ptr call(vector<Node::Ptr> args) {
    Env::Ptr env = make_shared<Env>(env_);
    for (size_t i = 0; i < parms_.size(); ++i) {
      auto k = static_cast<const Symbol*>(parms_[i].get())->value;
      env->Emplace(k, args[i]);
    }
    return eval(body_, env);
  }
};

Node::Ptr eval(Node::Ptr x, Env::Ptr env) {
  if (x->type == NodeType::symbol) {
    auto symbol = static_cast<const Symbol*>(x.get());
    return env->Find(symbol->value);
  } else if (x->type == NodeType::number) {
    return x;
  } else if (x->type == NodeType::list) {
    auto list = static_cast<const List*>(x.get());
    auto children = list->children;
    if (children.size() == 0) {
      return x;
    }
    auto first = static_cast<const Symbol*>(children[0].get());
    if (first->value == "quote") {
      return children[1];
    } else if (first->value == "if") {
      auto test = children[1];
      auto conseq = children[2];
      auto alt = children[3];
      if (eval(test, env) == Boolean::True) {
        return eval(conseq, env);
      } else {
        return eval(alt, env);
      }
    } else if (first->value == "define") {
      auto var = static_cast<const Symbol*>(children[1].get());
      auto exp = children[2];
      env->Emplace(var->value, eval(exp, env));
    } else if (first->value == "set!") {
      // TODO
    } else if (first->value == "lambda") {
      // TODO
    } else {
      auto func = static_cast<Func*>(eval(children[0], env).get());
      vector<Node::Ptr> args;
      for (auto start = children.begin() + 1; start != children.end(); ++start) {
        args.push_back(eval(*start, env));
      }
      return func->call(args);
    }
  } else if (x->type == NodeType::procedure) {
    // TODO
  }
  return x;
}


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


void printTree(const shared_ptr<Node> &node, const int &deep = 0) {
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

TEST_CASE("eval") {
  Env::Ptr env = standardEnv();

  unordered_map<string, double> testcases {
    {"(+ 1 2)", 3},
    {"(- 1 2)", -1},
    {"(* 3 2)", 6},
    {"(/ 6 2)", 3},
    {"(/ (+ 4 (+ 4 (* 2 (- 10 8)))) (* 2 3))", 2}
  };

  for (auto kv : testcases) {
    auto tokens = tokenize(kv.first);
    auto tree = parse(tokens);
    auto result = eval(tree, env);
    auto val = static_cast<const Number*>(result.get());
    REQUIRE(val->value == kv.second);
  }

}























