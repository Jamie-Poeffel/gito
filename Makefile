CXX = g++
CXXFLAGS = -Wall -std=c++17
LDFLAGS = -lcurl

SRCS = $(wildcard *.cpp) $(wildcard program/*.cpp)

WIN_BUILD = win-build
LIN_BUILD = lin-build

WIN_TARGET = $(WIN_BUILD)/gito.exe
LIN_TARGET = $(LIN_BUILD)/gito

WIN_OBJS = $(patsubst %.cpp, $(WIN_BUILD)/%.o, $(notdir $(SRCS)))
LIN_OBJS = $(patsubst %.cpp, $(LIN_BUILD)/%.o, $(notdir $(SRCS)))

all: run

run: $(WIN_TARGET) $(LIN_TARGET)

# --- Windows Build ---

$(WIN_BUILD):
	mkdir -p $(WIN_BUILD)

$(WIN_TARGET): $(WIN_OBJS)
	$(CXX) $(CXXFLAGS) -o $(WIN_TARGET) $(WIN_OBJS) $(LDFLAGS)

$(WIN_BUILD)/%.o: %.cpp | $(WIN_BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(WIN_BUILD)/%.o: program/%.cpp | $(WIN_BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@


# --- Linux Build ---

$(LIN_BUILD):
	mkdir -p $(LIN_BUILD)

$(LIN_TARGET): $(LIN_OBJS)
	$(CXX) $(CXXFLAGS) -o $(LIN_TARGET) $(LIN_OBJS) $(LDFLAGS)

$(LIN_BUILD)/%.o: %.cpp | $(LIN_BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(LIN_BUILD)/%.o: program/%.cpp | $(LIN_BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@


# --- Clean ---

clean:
	rm -rf $(WIN_BUILD) $(LIN_BUILD)

%:
	@:
