CXX = g++
CXXFLAGS = -Wall -std=c++17
TARGET = build/gito
LDFLAGS = -lcurl

SRCS = $(wildcard *.cpp) $(wildcard program/*.cpp)

OBJS = $(patsubst %.cpp, build/%.o, $(notdir $(SRCS)))

all: build $(TARGET)

build:
	mkdir -p build

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

build/%.o: %.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/%.o: program/%.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	@./$(TARGET) $(filter-out $@,$(MAKECMDGOALS))

%:
	@:

clean:
	rm -rf build
