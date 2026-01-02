CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude
TARGET = main
TEST_TARGET = test-tinysegmenter

.PHONY: all clean test run

all: $(TARGET)

$(TARGET): main.cxx
	$(CXX) $(CXXFLAGS) -o $@ $<

$(TEST_TARGET): test.cxx
	$(CXX) $(CXXFLAGS) -o $@ $<

test: $(TEST_TARGET)
	./$(TEST_TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(TEST_TARGET) test_header_only
