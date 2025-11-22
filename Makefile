# Compiler
LIN_CXX = g++
WIN_CXX = x86_64-w64-mingw32-g++

MODE ?= release

INCLUDES = -Iexternal
CXXFLAGS_RELEASE = -Wall -std=c++17 -O2 $(INCLUDES)
CXXFLAGS_DEBUG   = -Wall -std=c++17 -g -O0 $(INCLUDES)

WIN_INCLUDES = -Iexternal/win-curl/include

WIN_LIBS = \
    -Lexternal/win-curl/lib \
    -lcurl -lssl -lcrypto -lz \
    -lws2_32 -lwsock32

LDFLAGS = -lcurl

ifeq ($(MODE),debug)
    CXXFLAGS = $(CXXFLAGS_DEBUG)
    LIN_BUILD = lin-build-debug
    WIN_BUILD = win-build-debug
else
    CXXFLAGS = $(CXXFLAGS_RELEASE)
    LIN_BUILD = lin-build-release
    WIN_BUILD = win-build-release
endif

SRCS = $(wildcard *.cpp) $(wildcard program/*.cpp)

LIN_TARGET = $(LIN_BUILD)/gito
WIN_TARGET = $(WIN_BUILD)/gito.exe

LIN_OBJS = $(patsubst %.cpp, $(LIN_BUILD)/%.o, $(notdir $(SRCS)))
WIN_OBJS = $(patsubst %.cpp, $(WIN_BUILD)/%.o, $(notdir $(SRCS)))

all: run

# -------- RUN --------
run: $(WIN_TARGET) $(LIN_TARGET)

# -------- Linux Build --------
$(LIN_BUILD):
	mkdir -p $(LIN_BUILD)

$(LIN_TARGET): $(LIN_OBJS)
	$(LIN_CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(LIN_BUILD)/%.o: %.cpp | $(LIN_BUILD)
	$(LIN_CXX) $(CXXFLAGS) -c $< -o $@

$(LIN_BUILD)/%.o: program/%.cpp | $(LIN_BUILD)
	$(LIN_CXX) $(CXXFLAGS) -c $< -o $@

# -------- Windows Build --------
$(WIN_BUILD):
	mkdir -p $(WIN_BUILD)

$(WIN_TARGET): $(WIN_OBJS)
	$(WIN_CXX) -o $(WIN_TARGET) $(WIN_OBJS) $(WIN_LIBS) $(WIN_INCLUDES)

$(WIN_BUILD)/%.o: %.cpp | $(WIN_BUILD)
	$(WIN_CXX) $(CXXFLAGS) $(WIN_INCLUDES) -c $< -o $@

$(WIN_BUILD)/%.o: program/%.cpp | $(WIN_BUILD)
	$(WIN_CXX) $(CXXFLAGS) $(WIN_INCLUDES) -c $< -o $@

clean:
	rm -rf lin-build-* win-build-*

%:
	@:
