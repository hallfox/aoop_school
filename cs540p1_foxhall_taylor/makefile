CC := clang++
# CC := clang --analyze # and comment out the linker last line for sanity
TARGET := deque
DISTNAME := cs540p1_foxhall_taylor
SRCEXT := cpp
HEADEREXT := hpp
CFLAGS := -g -Wall -std=c++11 -Wextra -pedantic

SOURCES := $(shell find . -type f -name "*.$(SRCEXT)")
OBJECTS := $(patsubst %.c,%.o,$(SOURCES))
HEADERS := $(shell find . -type f -name "*.$(HEADEREXT)")

$(TARGET): $(OBJECTS)
	@echo " Linking..."
	@echo " $(CC) $^ -o $(TARGET)"; $(CC) $^ -o $(TARGET)

%.o: %.$(SRCEXT)
	@echo " $(CC) $(CFLAGS) -c -o $@ $<"; $(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo " Cleaning...";
	@echo " $(RM) *.o $(TARGET)"; $(RM) *.o $(TARGET)

dist:
	@echo " Taring source files...";
	@echo " tar czf $(DISTNAME).tgz $(SOURCES) $(HEADERS) README makefile"; tar czf $(DISTNAME).tgz $(SOURCES) $(HEADERS) README makefile

.PHONY: clean
