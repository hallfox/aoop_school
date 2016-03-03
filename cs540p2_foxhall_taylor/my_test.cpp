// #include "Map.hpp"

#include <iostream>
#include <random>

// template class cs540::Map<int, int>;

static const int MAX_SKIP_LIST_HEIGHT = 20;
static const int seed = std::chrono::system_clock::now().time_since_epoch().count();

int rand_height() {
  std::default_random_engine gen(seed);
  std::geometric_distribution<int> dist(0.5);
  // while (dist(gen) && h < MAX_SKIP_LIST_HEIGHT) h++;

  return std::min(dist(gen)+1, MAX_SKIP_LIST_HEIGHT);
}

void test_rand_height() {
  std::cout << "Max skip list height is: " << MAX_SKIP_LIST_HEIGHT << std::endl;
  std::cout << "Here are some random heights (geometric distribution p = 1/2):\n";

  std::cout << std::string(MAX_SKIP_LIST_HEIGHT, '-') << std::endl;
  for (int i = 0; i < 100; i++) {
    std::cout << std::string(rand_height(), '*') << std::endl;
  }
  std::cout << std::string(MAX_SKIP_LIST_HEIGHT, '-') << std::endl;
}

class A {
public:
  A(int a): x(a) {}
  virtual int& get_x() {
    return x;
  }

protected:
  int x;
};

class B: public A {
  B(int a): A(a) {}
  const int& get_x() {
    return A::x;
  }
};

int main() {
  return 0;
}
