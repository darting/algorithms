#include <iostream>
#include <vector>
#include <algorithm>

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

using namespace std;

int max_sub_sum(const vector<int> &a) {
  int max = 0;
  for (size_t i = 0; i < a.size(); ++i) {
    for (size_t j = i; j < a.size(); ++j) {
      int currSum = 0;
      for (size_t k = i; k < j; ++k) {
        currSum += a[k];
      }
      if (max < currSum) {
        max = currSum;
      }
    }
  }
  return max;
}

int max_sub_sum2(const vector<int> &a) {
  int max = 0;
  for (size_t i = 0; i < a.size(); ++i) {
    int sum = 0;
    for (size_t j = i; j < a.size(); ++j) {
      sum += a[j];
      if (sum > max) {
        max = sum;
      }
    }
  }
  return max;
}

int max_sub_sum3(const vector<int> &a) {
  function<int(int, int)> rec = [&a, &rec] (int start, int end) -> int {
    if (start == end) {
      if (a[start] < 0) {
        return 0;
      } else {
        return a[start];
      }
    } else {
      int middle = (start + end) / 2;
      int left = rec(start, middle);
      int right = rec(middle + 1, end);
      int left_middle = 0, tmp = 0;
      for (int i = middle; i >= start; --i) {
        tmp += a[i];
        if (tmp > left_middle) {
          left_middle = tmp;
        }
      }
      int right_middle = 0;
      tmp = 0;
      for (int i = middle + 1; i <= end; ++i) {
        tmp += a[i];
        if (tmp > right_middle) {
          right_middle = tmp;
        }
      }
      return max(max(left, right), left_middle + right_middle);
    }
  };

  return rec(0, a.size() - 1);
}

int max_sub_sum4(const vector<int> &a) {
  int max = 0, curr = 0;
  for (size_t j = 0; j < a.size(); ++j) {
    curr += a[j];
    if (curr > max) {
      max = curr;
    } else if (curr < 0) {
      curr = 0;
    }
  }
  return max;
}

// -2, 11, -4, 13, -5, -2 => 20
// 4, -3, 5, -2, -1, 2, 6, -2 => 11

TEST_CASE( "max subsequence sum O(N^3)", "[max_sub_sum]" ) {
  REQUIRE(max_sub_sum({ -2, 11, -4, 13, -5, -2 }) == 20);
  REQUIRE(max_sub_sum({ 4, -3, 5, -2, -1, 2, 6, -2 }) == 11);
}

TEST_CASE( "max subsequence sum 2 O(N^2)", "[max_sub_sum2]" ) {
  REQUIRE(max_sub_sum2({ -2, 11, -4, 13, -5, -2 }) == 20);
  REQUIRE(max_sub_sum2({ 4, -3, 5, -2, -1, 2, 6, -2 }) == 11);
}

TEST_CASE( "max subsequence sum 3", "[max_sub_sum3]" ) {
  REQUIRE(max_sub_sum3({ -2, 11, -4, 13, -5, -2 }) == 20);
  REQUIRE(max_sub_sum3({ 4, -3, 5, -2, -1, 2, 6, -2 }) == 11);
}

TEST_CASE( "max subsequence sum 4", "[max_sub_sum4]" ) {
  REQUIRE(max_sub_sum4({ -2, 11, -4, 13, -5, -2 }) == 20);
  REQUIRE(max_sub_sum4({ 4, -3, 5, -2, -1, 2, 6, -2 }) == 11);
}
