
CXX = g++
CXXFLAGS = -std=c++17 -Wall -I./include -O3
LDFLAGS = 

# Source files and objects
LIB_SOURCES = src/BuddyAllocator.cpp
LIB_OBJECTS = $(LIB_SOURCES:.cpp=.o)

TEST_SOURCES = tests/tests.cpp src/myClass.cpp
TEST_OBJECTS = $(TEST_SOURCES:.cpp=.o)
TEST_EXECUTABLE = tests/test

# Compile BuddyAllocator as a static library
libbuddy.a: $(LIB_OBJECTS)
	ar rcs $@ $^

# Compile BuddyAllocator as a shared library
libbuddy.so: CXXFLAGS += -fPIC
libbuddy.so: $(LIB_OBJECTS)
	$(CXX) -shared -o $@ $^

# Compile and link the tests against the static library
test_static: libbuddy.a $(TEST_OBJECTS)
	$(CXX) -o $(TEST_EXECUTABLE) $(TEST_OBJECTS) -L. -lbuddy

# Compile and link the tests against the shared library
test_shared: libbuddy.so $(TEST_OBJECTS)
	$(CXX) -o $(TEST_EXECUTABLE) $(TEST_OBJECTS) -L. -lbuddy -Wl,-rpath=.

# Clean build artifacts
clean:
	rm -f src/*.o tests/*.o libbuddy.a libbuddy.so $(TEST_EXECUTABLE)

.PHONY: clean
