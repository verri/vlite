HEADERS := $(shell find vlite -name \*.hpp)

CXX = g++
CXXFLAGS = -std=c++1z -Wall -Wextra -pedantic -O2 -isystem third_party -isystem.

all: test

ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
-include $(DEP)
endif

test: test_suite
	valgrind ./test_suite

test_suite: test_suite.o
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

test_suite.o: test_suite.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

format:
	clang-format -i -style=file $(SRC) $(HEADERS)

tidy:
	clang-tidy $(HEADERS) -fix -fix-errors -- -std=c++1z -I.

clean:
	find . -name '*.[od]' -exec rm {} \;

.PHONY: format test clean tidy
