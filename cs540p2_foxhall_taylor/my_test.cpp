#include "Map.hpp"

#include <iostream>
#include <random>

class A {
public:
  A() = default;
  A(int xx): x(xx) {}
  friend bool operator==(const A& me, const A& other) { return me.x == other.x; }
  friend bool operator==(const A& me, const A& other) { return me.x == other.x; }
private:
  int x;
};

template class cs540::Map<std::string, std::string>;

static const int MAX_SKIP_LIST_HEIGHT = 20;
static const int seed = std::chrono::system_clock::now().time_since_epoch().count();
static std::default_random_engine gen(seed);
static std::geometric_distribution<int> dist(0.5);

int rand_height() {
  return std::min(dist(gen)+1, MAX_SKIP_LIST_HEIGHT);
}

void test_rand_height() {
  std::cout << "Max skip list height is: " << MAX_SKIP_LIST_HEIGHT << std::endl;
  std::cout << "Here are some random heights (geometric distribution p = 1/2):\n";

  std::cout << std::string(MAX_SKIP_LIST_HEIGHT, '-') << std::endl;
  for (int i = 0; i < 50; i++) {
    std::cout << std::string(rand_height(), '*') << std::endl;
  }
  std::cout << std::string(MAX_SKIP_LIST_HEIGHT, '-') << std::endl;
}

int main() {
  test_rand_height();
  return 0;
}
