#include <iostream>
#include <stack>
#include <tuple>

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

using namespace std;


vector<int> make_players(const int &N) {
  vector<int> players(N);
  for (int i = 0; i < N; ++i) {
    players[i] = (i + 1);
  }
  return players;
}

tuple<vector<int>, int> josephus1(int M, int N) {
  vector<int> out;
  auto players = make_players(N);
  auto pos = 0;
  while(players.size() > 1) {
    pos += M;
    pos = pos % players.size();
    out.push_back(players[pos]);
    players.erase(players.begin() + pos);
  }
  return make_tuple(out, players.front());
}


// M=0, N=5
// 1, 2, 3, 4, 5
// 1, 2, 3, 4 => 5

// M=1, N=5
// 1, 2, 3, 4, 5
// 2, 4, 1, 5 => 3

TEST_CASE( "josephus1 - 1" ) {
  auto result = josephus1(1, 5);
  vector<int> expect { 2, 4, 1, 5 };
  REQUIRE(get<0>(result) == expect);
  REQUIRE(get<1>(result) == 3);
}

TEST_CASE( "josephus1 - 2" ) {
  auto result = josephus1(0, 5);
  vector<int> expect { 1, 2, 3, 4 };
  REQUIRE(get<0>(result) == expect);
  REQUIRE(get<1>(result) == 5);
}









