CXX = g++
CPPFLAGS = -std=c++11 -g -pthread -Wall -Wextra -pedantic

shared: SharedPtr.hpp SharedPtr_test.cpp
	g++ $(CPPFLAGS) SharedPtr_test.cpp -o $@
