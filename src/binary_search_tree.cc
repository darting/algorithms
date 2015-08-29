#include <iostream>
#include <memory>
#include <random>

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

using namespace std;


// http://herbsutter.com/gotw/_102/
template<typename T, typename ...Args>
std::unique_ptr<T> make_unique1( Args&& ...args ) {
    return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
}


template<typename ValueType>
struct TreeNode {
  ValueType value;
  int deep;
  unique_ptr<TreeNode> left;
  unique_ptr<TreeNode> right;
  TreeNode(const ValueType &v, const int d)
  : value(v),
    deep(d),
    left(nullptr),
    right(nullptr) {}
};


template<typename Comparable>
class BinarySearchTree {

public:
  typedef Comparable ValueType;
  typedef TreeNode<ValueType> Node;

  BinarySearchTree() {}

  void Insert(const ValueType &value) {
    Insert(value, root_, 0);
  }

  void Remove(const ValueType &value) {
    Remove(value, root_);
  }

  const unique_ptr<Node> &root() const {
    return root_;
  }

  void Print() {
    Print(root_);
  }

  void Print2() {
    Print2(root_);
  }

private:

  unique_ptr<Node> root_;

  void Insert(const ValueType &value, unique_ptr<Node> &node, const int &deep) {
    if (!node) {
      node = make_unique1<Node>(value, deep);
    } else if (value < node->value) {
      Insert(value, node->left, deep + 1);
    } else if (value > node->value) {
      Insert(value, node->right, deep + 1);
    }
  }

  void Remove(const ValueType &value, const unique_ptr<Node> &node) {

  }

  void Print(const unique_ptr<Node> &node) {
    if (!node) return;

    for (int i = 0; i < node->deep; ++i) {
      cout << (i == 0 ? "┠" : "─");
    }
    cout << " " << node->value << endl;

    Print(node->left);
    Print(node->right);
  }

  void Print2(const unique_ptr<Node> &node) {
    if (!node) return;

    Print2(node->left);
    cout << node->value << " ";
    Print2(node->right);
  }

};


TEST_CASE( "Print" ) {
  random_device rd;
  mt19937 gen(rd());
  int max = 10;
  uniform_int_distribution<> dist(1, max);
  BinarySearchTree<int> tree;
  for (int i = 0; i < max; ++i) {
    tree.Insert(dist(gen));
  }
  tree.Print();
  cout << endl;
  tree.Print2();
  cout << endl;
}

TEST_CASE("Insert") {
  //        5 
  //      /   \
  //     2     8
  //    / \   /  
  //   1   3 6    
  BinarySearchTree<int> tree;
  tree.Insert(5);
  tree.Insert(2);
  tree.Insert(8);
  tree.Insert(6);
  tree.Insert(3);
  tree.Insert(1);
  
  REQUIRE(tree.root()->value == 5);
  REQUIRE(tree.root()->left->value == 2);
  REQUIRE(tree.root()->left->left->value == 1);
  REQUIRE(tree.root()->left->right->value == 3);
  REQUIRE(tree.root()->right->value == 8);
  REQUIRE(tree.root()->right->left->value == 6);
}




