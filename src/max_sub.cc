#include <iostream>
#include <vector>

using namespace std;


int max_sub_sum(const vector<int> &a) {
  int max = 0;
  for (int i = 0; i < a.size(); ++i) {
    for (int j = i; j < a.size(); ++j) {
      int currSum = 0;
      for (int k = i; k < j; ++k) {
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
  for (int i = 0; i < a.size(); ++i) {
    int sum = 0;
    for (int j = i; j < a.size(); ++j) {
      if (sum < sum + a[j]) {
        sum += a[j];
      }
    }
    if (sum > max) {
      max = sum;
    }
  }
  return max;
}

int main(int argc, char const *argv[]) {
  // -2, 11, -4, 13, -5, -2
  // => max sub sum = 20
  std::vector<int> v { -2, 11, -4, 13, -5, -2 };

  int sum = max_sub_sum(v);
  cout << sum << endl;

  return 0;
}