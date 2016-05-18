CXX = g++
CPPFLAGS = -std=c++14 -g -pthread -Wall -Wextra -pedantic

all: shared myinterp interp

shared: SharedPtr.hpp SharedPtr_test.cpp
	g++ $(CPPFLAGS) SharedPtr_test.cpp -o $@

myinterp: Interpolate.hpp Interpolate.cpp
	g++ $(CPPFLAGS) Interpolate.cpp -o $@

interp: Interpolate.hpp Interpolate_test.cpp
	g++ $(CPPFLAGS) Interpolate_test.cpp -o $@
