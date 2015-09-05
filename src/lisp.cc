#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <functional>
#include <sstream>

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
  virtual ~Node() {}
  virtual string TypeName() = 0;
};

class Number : public Node {
public:
  double value;
  Number(const double &v) : value(v) {
    type = NodeType::number;
  }
  string TypeName() override { return "number"; }
};

class Symbol : public Node {
public:
  string value;
  Symbol(const string &s) : value(s) {
    type = NodeType::symbol;
  }
  string TypeName() override { return "symbol"; }
};

class List : public Node {
public:
  vector<shared_ptr<Node>> children;
  List() {
    type = NodeType::list;
  }
  string TypeName() override { return "list"; }
};

class Boolean : public Node {
public:
  bool value;
  Boolean(bool v) : value(v) {
    type = NodeType::boolean;
  }
  string TypeName() override { return "boolean"; }
  explicit operator bool() const {
    return value;
  }
  static const Node::Ptr True;
  static const Node::Ptr False;
};

const Node::Ptr Boolean::True = make_shared<Boolean>(true);
const Node::Ptr Boolean::False = make_shared<Boolean>(false);

class Func : public Node {
public:
  Func() {
    type = NodeType::func;
  }
  string TypeName() override { return "fun"; }
  virtual ~Func() {}
  virtual Node::Ptr call(const vector<Node::Ptr> &args) = 0;
};

class Arithmetic : public Func {
public:
  typedef function<double(double, double)> Handler;
  Arithmetic(Handler h) : handler_(h) {}
  string TypeName() override { return "func-arithmetic"; }
  Node::Ptr call(const vector<Node::Ptr> &args) override {
    double state = static_pointer_cast<Number>(args[0])->value;
    for (auto it = args.begin() + 1; it != args.end(); ++it) {
      state = handler_(state, static_pointer_cast<Number>(*it)->value);
    }
    return make_shared<Number>(state);
  }
private:
  Handler handler_;
};

class Comparison : public Func {
public:
  typedef function<bool(double, double)> Handler;
  Comparison(Handler h) : handler_(h) {}
  string TypeName() override { return "func-comparision"; }
  Node::Ptr call(const vector<Node::Ptr> &args) override {
    auto l = static_pointer_cast<Number>(args[0])->value;
    auto r = static_pointer_cast<Number>(args[1])->value;
    return handler_(l, r) ? Boolean::True : Boolean::False;
  }
private:
  Handler handler_;
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

  void Set(const KeyType &key, ValueType node) {
    if (Find(key)) {
      values_.emplace(key, node);
    } else if(outer_) {
      outer_->Set(key, node);
    }
  }
};

Env::Ptr standardEnv() {
  Env::Ptr env = make_shared<Env>();
  env->Emplace("+", make_shared<Arithmetic>([](auto state, auto number) { return state + number; }));
  env->Emplace("-", make_shared<Arithmetic>([](auto state, auto number) { return state - number; }));
  env->Emplace("*", make_shared<Arithmetic>([](auto state, auto number) { return state * number; }));
  env->Emplace("/", make_shared<Arithmetic>([](auto state, auto number) { return state / number; }));
  
  env->Emplace(">", make_shared<Comparison>([](auto l, auto r) { return l > r; }));
  env->Emplace("<", make_shared<Comparison>([](auto l, auto r) { return l < r; }));
  env->Emplace("==", make_shared<Comparison>([](auto l, auto r) { return l == r; }));
  env->Emplace("!=", make_shared<Comparison>([](auto l, auto r) { return l != r; }));
  env->Emplace(">=", make_shared<Comparison>([](auto l, auto r) { return l >= r; }));
  env->Emplace("<=", make_shared<Comparison>([](auto l, auto r) { return l <= r; }));

  return env;
}


Node::Ptr eval(Node::Ptr x, Env::Ptr env);

class Procedure : public Func {
private:
  Node::Ptr parms_;
  Node::Ptr body_;
  Env::Ptr env_;
public:
  Procedure(Node::Ptr parms, Node::Ptr body, Env::Ptr env)
  : parms_(parms),
    body_(body),
    env_(env) {
      type = NodeType::procedure;
    }
  Node::Ptr call(const vector<Node::Ptr> &args) override {
    Env::Ptr env = make_shared<Env>(env_);
    auto parms = static_pointer_cast<List>(parms_)->children;
    for (size_t i = 0; i < parms.size(); ++i) {
      auto k = static_pointer_cast<Symbol>(parms[i])->value;
      env->Emplace(k, args[i]);
    }
    return eval(body_, env);
  }
};

Node::Ptr eval(Node::Ptr x, Env::Ptr env) {
  if (x->type == NodeType::symbol) {
    auto symbol = static_pointer_cast<Symbol>(x);
    return env->Find(symbol->value);
  } else if (x->type == NodeType::number) {
    return x;
  } else if (x->type == NodeType::list) {
    auto list = static_pointer_cast<List>(x);
    auto children = list->children;
    if (children.size() == 0) {
      return x;
    }
    auto first = static_pointer_cast<Symbol>(children[0]);
    if (first->value == "quote") {
      return children[1];
    } else if (first->value == "if") {
      auto test = children[1];
      auto conseq = children[2];
      auto alt = children[3];
      auto cond = eval(test, env);
      if (cond == Boolean::True) {
        return eval(conseq, env);
      } else if (cond == Boolean::False) {
        return eval(alt, env);
      }
    } else if (first->value == "define") {
      auto var = static_pointer_cast<Symbol>(children[1]);
      auto exp = children[2];
      env->Emplace(var->value, eval(exp, env));
    } else if (first->value == "set!") {
      auto var = static_pointer_cast<Symbol>(children[1]);
      auto exp = children[2];
      env->Set(var->value, eval(exp, env));
    } else if (first->value == "lambda") {
      auto parms = children[1];
      auto body = children[2];
      return make_shared<Procedure>(parms, body, env);
    } else {
      auto func = static_pointer_cast<Func>(eval(children[0], env));
      vector<Node::Ptr> args;
      for (auto start = children.begin() + 1; start != children.end(); ++start) {
        args.push_back(eval(*start, env));
      }
      return func->call(args);
    }
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
             *end != ')' &&
             end != program.end()) {
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

auto execute = [](string program, Env::Ptr env) {
  auto tokens = tokenize(program);
  auto tree = parse(tokens);
  return eval(tree, env);
};


void printTree(const shared_ptr<Node> &node, const int &deep = 0) {
  for (int i = 0; i < deep; ++i) {
    cout << " ";
  }
  if (node->type == NodeType::list) {
    auto list = static_pointer_cast<List>(node);
    if (list->children.size() > 0) {
      cout << list->TypeName() << ": " << static_pointer_cast<Symbol>(list->children[0])->value << endl;
      for (size_t i = 1; i < list->children.size(); ++i) {
        printTree(list->children[i], deep + 1);
      }
    } else {
      cout << list->TypeName() << ": Nil" << endl;
    }
  } else if (node->type == NodeType::number) {
    auto number = static_pointer_cast<Number>(node);
    cout << number->TypeName() << ": " << number->value << endl;
  } else if (node->type == NodeType::symbol) {
    auto symbol = static_pointer_cast<Symbol>(node);
    cout << symbol->TypeName() << ": " << symbol->value << endl;
  }
}





TEST_CASE("tokenize") {

  auto tokens = tokenize(R"(
    (if (> (val x) 0)
       (fn (+ (aref A i) 1)
        (quote (one two))))
    )");

  vector<string> expect { "(", "if", "(", ">", "(", "val", "x", ")", "0", ")", "(", "fn", "(", "+", 
                          "(", "aref", "A", "i", ")", "1", ")", "(", "quote", "(", "one", "two", ")", 
                          ")", ")", ")" };
  for (auto s : expect) {
    REQUIRE(tokens.front() == s);
    tokens.pop();
  }
}

TEST_CASE("parse") {

  auto tokens = tokenize("(define fact (lambda (n) (if (<= n 1) 1 (* n (fact (- n 1))))))");
  auto tree = parse(tokens);

  printTree(tree, 0);
}

TEST_CASE("arithmetic") {
  Env::Ptr env = standardEnv();

  unordered_map<string, double> test_arithmetic {
    {"(+ 1 2)", 3},
    {"(+ 1 2 3 4 5)", 1 + 2 + 3 + 4 + 5},
    {"(- 1 2)", -1},
    {"(- 1 2 3 4 5)", 1 - 2 - 3 - 4 - 5},
    {"(* 3 2)", 6},
    {"(* 1 2 3 4 5)", 1 * 2 * 3 * 4 * 5},
    {"(/ 6 2)", 3},
    {"(/ 1 2 3 4 5)", 1.0 / 2 / 3 / 4 / 5},
    {"(/ (+ 4 (+ 4 (* 2 (- 10 8)))) (* 2 3))", 2}
  };

  for (auto kv : test_arithmetic) {
    auto result = execute(kv.first, env);
    auto val = static_pointer_cast<Number>(result);
    REQUIRE(val->value == kv.second);
  }
}


TEST_CASE("comparsion") {
  Env::Ptr env = standardEnv();

  unordered_map<string, bool> test_comparison {
    {"(> 1 2)", false},
    {"(< 1 2)", true},
    {"(== 2 2)", true},
    {"(== 21 2)", false},
    {"(!= 21 2)", true},
    {"(>= 1 2)", false},
    {"(<= 1 2)", true},
  };

  for (auto kv : test_comparison) {
    auto result = execute(kv.first, env);
    auto val = static_pointer_cast<Boolean>(result);
    REQUIRE(val->value == kv.second);
  }
}

auto shouldBe = [](Env::Ptr &env, string program, double expect) {
  auto result = execute(program, env);
  if (result->type != NodeType::number) {
    printTree(result);
    stringstream ss;
    ss << "type of result is not NodeType::number :\n" 
       << "expression > " << program << "\n"
       << "type       > " << result->TypeName() << "\n"
       << "expect     > " << expect;
    FAIL(ss.str());
  }
  auto number = static_pointer_cast<Number>(result);
  CHECK(number->value == expect);
};

TEST_CASE("condition") {
  Env::Ptr env = standardEnv();

  shouldBe(env, "(if (< 1 2) 1 2)", 1);
  shouldBe(env, "(if (> 1 2) 1 2)", 2);
}

TEST_CASE("lambda") {
  Env::Ptr env = standardEnv();
  execute("(define add (lambda (x y) (+ x y)))", env);

};

TEST_CASE("procedure") {

  Env::Ptr env = standardEnv();

  unordered_map<string, bool> testcase {
    {"(if (< 1 2) 1 2)", false},
    {"(< 1 2)", true},
    {"(== 2 2)", true},
    {"(== 21 2)", false},
    {"(!= 21 2)", true},
    {"(>= 1 2)", false},
    {"(<= 1 2)", true},
  };

  execute("(define pi 3.14)", env);
  shouldBe(env, "pi", 3.14);

  execute("(define circle-area (lambda (r) (* pi (* r r))))", env);
  shouldBe(env, "(circle-area 3)", 28.26);

  execute("(define fact (lambda (n) (if (<= n 1) 1 (* n (fact (- n 1))))))", env);
  shouldBe(env, "(fact 10)", 3628800);
  shouldBe(env, "(circle-area (fact 10))", 41348114841600.0);

  execute("(define count-down-from (lambda (n) (lambda () (set! n (- n 1)))))", env);
  execute("(define count-down-from-3 (count-down-from 3))", env);
  // execute("(define count-down-from-4 (count-down-from 4))", env);
  shouldBe(env, "(count-down-from-3)", 2);
  // shouldBe("(count-down-from-4)", 3);
  // shouldBe("(count-down-from-3)", 1);
  // shouldBe("(count-down-from-3)", 0);
  // shouldBe("(count-down-from-4)", 2);
  // shouldBe("(count-down-from-4)", 1);
  // shouldBe("(count-down-from-4)", 0);
}























