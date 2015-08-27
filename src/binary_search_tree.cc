#include <iostream>

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

using namespace std;


template<typename Comparable>
class BinarySearchTree {

public:

  BinarySearchTree() : root_(nullptr) {}

  ~BinarySearchTree();

  const Comparable &min() const {
    return min(root_);
  }
  
  const Comparable &max() const {
    return max(root_);
  }

  bool contains(const Comparable &x) const {
    return contains(x, root_);
  }

  bool is_empty() const { return root_ == nullptr; }

  void print() const {}

  void insert(const Comparable &x) {
    insert(x, root_);
  }

  void remove(const Comparable &x) {

  }

private:
  
  struct BinaryNode {
    Comparable element;
    BinaryNode *left;
    BinaryNode *right;
    BinaryNode(const Comparable &e, BinaryNode *l, BinaryNode *r) 
      : element(e),
        left(l),
        right(r) {}
  };

  BinaryNode *root_;

  const Comparable &min(BinaryNode *root) const {
    if (root == nullptr) {
      return nullptr;
    }
    if (root->left == nullptr) {
      return root;
    }
    return min(root->left);
  } 

  const Comparable &max(BinaryNode *root) const {
    if (root == nullptr) return nullptr;
    if (root->right == nullptr) return root;
    return max(root->right);
  }

  bool contains(const Comparable &x, BinaryNode *root) const {
    if (root == nullptr) {
      return false;
    } else if (x < root->element) {
      return contains(x, root->left);
    } else if (x > root->element) {
      return contains(x, root->right);
    } else {
      return true;
    }
  }

  void insert(const Comparable &x, BinaryNode *root) {
    if (root != nullptr) {
      if (x < root->element) {
        if (root->left == nullptr) {
          root->left = new BinaryNode(x, nullptr, nullptr);
        } else {
          insert(x, root->left);
        }
      } else if (x > root->element) {
        if (root->right == nullptr) {
          root->right = new BinaryNode(x, nullptr, nullptr);
        } else {
          insert(x, root->right);
        }
      }
    }
  }

};






