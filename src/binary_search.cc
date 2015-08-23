#include <iostream>
#include <vector>
#include <algorithm>

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

using namespace std;

int binary_search(const vector<int> &a, const int &x) {
  function<int(const int&, const int&)> rec = [&a, &x, &rec] (const int &start, const int &end) {
    int middle = (start + end) / 2;
    int val = a[middle];
    if (val == x) {
      return middle;
    } else if (middle == start) {
      return -1;
    } else if (val > x) {
      return rec(start, middle);
    } else {
      return rec(middle + 1, end);
    }
  };
  return rec(0, a.size());
}

int binary_search2(const vector<int> &a, const int &x) {
  int start = 0, end = a.size();
  while (start < end) {
    int middle = (start + end) / 2;
    int val = a[middle];
    if (val == x) {
      return middle;
    } else if (val > x) {
      end = middle;
    } else {
      start = middle + 1;
    }
  }
  return -1;
}

TEST_CASE( "binary search", "[binary search]" ) {
  REQUIRE(binary_search({ 1, 2, 3, 4, 5, 6 }, 2) == 1);
  REQUIRE(binary_search({ 1, 2, 3, 4, 5, 6 }, 1) == 0);
  REQUIRE(binary_search({ 1, 2, 3, 4, 5, 6 }, 5) == 4);
  REQUIRE(binary_search({ 1, 2, 3, 4, 5, 6 }, 6) == 5);
  REQUIRE(binary_search({ 1, 2, 3, 4, 5, 6 }, 0) == -1);
  REQUIRE(binary_search({ 1, 2, 3, 4, 5, 6 }, 7) == -1);
}

TEST_CASE( "binary search 2", "[binary search 2]" ) {
  REQUIRE(binary_search2({ 1, 2, 3, 4, 5, 6 }, 2) == 1);
  REQUIRE(binary_search2({ 1, 2, 3, 4, 5, 6 }, 1) == 0);
  REQUIRE(binary_search2({ 1, 2, 3, 4, 5, 6 }, 5) == 4);
  REQUIRE(binary_search2({ 1, 2, 3, 4, 5, 6 }, 6) == 5);
  REQUIRE(binary_search2({ 1, 2, 3, 4, 5, 6 }, 0) == -1);
  REQUIRE(binary_search2({ 1, 2, 3, 4, 5, 6 }, 7) == -1);
}
