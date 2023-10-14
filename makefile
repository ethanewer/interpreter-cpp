CXX = g++
CXXFLAGS = -std=c++11 -Wall
SRC_FILES = $(wildcard *.cpp)
OBJ_FILES = $(SRC_FILES:.cpp=.o)
EXEC = main

all: $(EXEC)

$(EXEC): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(EXEC) $(OBJ_FILES)

.PHONY: all clean