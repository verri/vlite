SUFFIXES += .d

HEADERS := $(shell find vlite -name \*.hpp)
SRC := test_suite.cpp
OBJ := $(SRC:.cpp=.o)
DEP := $(SRC:.cpp=.d)

CXX = g++
CXXFLAGS = -std=c++1z -Wall -Wextra -pedantic -O2 -isystem third_party -isystem.

all: test

ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
-include $(DEP)
endif

test: test_suite
	valgrind ./test_suite

test_suite: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.d: %.cpp
	$(CXX) $(CXXFLAGS) -MM -MT '$(patsubst %.cpp,%.o,$<)' $< -MF $@

format:
	clang-format -i -style=file $(SRC) $(HEADERS)

tidy:
	clang-tidy $(HEADERS) -fix -fix-errors -- -std=c++1z -I.

clean:
	find . -name '*.[od]' -exec rm {} \;

.PHONY: format test clean tidy
