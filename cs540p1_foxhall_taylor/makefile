CC := g++
# CC := clang --analyze # and comment out the linker last line for sanity
TARGET := deque
DISTNAME := cs540p1_foxhall_taylor
SRCEXT := cpp
HEADEREXT := hpp
CFLAGS := -g3 -gdwarf-2 -Wall -std=c++11 -Wextra -pedantic
LIB := -ldl

SOURCES := $(shell find . -type f -name "*.$(SRCEXT)")
OBJECTS := $(patsubst %.$(SRCEXT),%.o,$(SOURCES))
HEADERS := $(shell find . -type f -name "*.$(HEADEREXT)")

$(TARGET): $(OBJECTS)
	@echo " Linking..."
	@echo " $(CC) $(LIB) $^ -o $(TARGET)"; $(CC) $(LIB) $^ -o $(TARGET)

%.o: %.$(SRCEXT)
	@echo " $(CC) $(CFLAGS) -c -o $@ $<"; $(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo " Cleaning...";
	@echo " $(RM) *.o $(TARGET)"; $(RM) *.o $(TARGET)

dist:
	@echo " Taring source files...";
	@echo " tar czf $(DISTNAME).tgz $(SOURCES) $(HEADERS) README makefile"; tar czf $(DISTNAME).tgz $(SOURCES) $(HEADERS) README makefile

.PHONY: clean
