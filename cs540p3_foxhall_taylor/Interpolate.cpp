#include "Interpolate.hpp"

#include <sstream>

using namespace cs540;

int main() {
  // Test argument lengths
  std::cout << Interpolate("Test ") << std::endl;
  std::cout << Interpolate("Test %", 32) << std::endl;
  std::cout << Interpolate("Test % happy %", 32, "one MILLION dollars") << std::endl;
  std::cout << Interpolate("Test % happy % GabeN %", 32, "one MILLION dollars", 2.9) << std::endl;
  std::cout << Interpolate("Test %%%", 'm', 'e', "me") << std::endl;

  // Test mismatched arguments
  try {
    std::cout << Interpolate("Test ", 56) << std::endl;
  } catch (WrongNumberOfArgs e) {
    std::cout << e.what() << std::endl;
  }
  try {
    std::cout << Interpolate("Test %") << std::endl;
  } catch (WrongNumberOfArgs e) {
    std::cout << e.what() << std::endl;
  }
  try {
    std::cout << Interpolate("Test %%%", 'm', 'e', "me", 33) << std::endl;
  } catch (WrongNumberOfArgs e) {
    std::cout << e.what() << std::endl;
  }

  // Test backslash
  std::cout << Interpolate(R"(I have 300\% more confidence in \dank memes due to the recent 500\% decrease in rare pepes)") << std::endl;
  auto a = Interpolate(R"(Haskell is for \%NOOBS\%, dirty % pureists)", "pleb");
  std::stringstream ss;
  ss << a;
  std::cout << ss.str() << std::endl;
  assert(ss.str() == "Haskell is for %NOOBS%, dirty pleb pureists");

  // ASSAULT
  std::cout << Interpolate(R"(%%\%\\%%%\%\%\%%%)", 0, 1, 2, 3, 4, 5) << std::endl;

  // Windows environment variables
  std::cout << Interpolate(R"(\%%\%;\%%\%;)", "HOME", "SYSTEM") << std::endl;

  // Test manip
  std::cout << Interpolate("Twenty is equal to %, and equal to %! %", 20, std::hex, 20, ffr(std::flush), "GREAT!", ffr(std::flush)) << std::endl;
  std::cout << std::dec;
  std::cout << Interpolate("Twenty is equal to %, and equal to %!%%", 20, std::setbase(16), 20, ffr(std::endl), "GREAT!") << std::endl;
  std::cout << Interpolate("%", "std::flush\n", ffr(std::flush));

  return 0;
}
