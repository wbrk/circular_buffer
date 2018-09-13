CXX := g++
CFLAGS := -Wall -Wextra
TARGET := test

SRC := $(wildcard *.cpp)

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) -o $@ $^ $(CFLAGS)

clean:
	rm $(TARGET)

.PHONY: all clean
