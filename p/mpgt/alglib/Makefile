NAME := alglib
MAJOR := 3
MINOR := 15
VERSION := $(MAJOR).$(MINOR)
SHARED_LIB := lib/lib$(NAME).so
STATIC_LIB := lib/lib$(NAME).a
CXX = g++

SRC := $(wildcard src/*.cpp)
OBJ := $(SRC:.cpp=.o)
DEP := $(OBJ:.o=.d)

# preprocessor flags
CPPFLAGS := -I./include/ -fopenmp 
# C++ compiler flag
CXXFLAGS := $(shell root-config --cflags) -O3 -std=c++11 -ftemplate-depth=2048 -fPIC -Wno-literal-suffix -pg -DAE_CPU=AE_INTEL
# Linker parameter flags
LDFLAGS  := -fopenmp
# Linker library flags
LDLIBS   := 


.PHONY: all debug clean shared static

all: $(SHARED_LIB) $(STATIC_LIB)

shared: $(SHARED_LIB)

static: $(STATIC_LIB)

debug: CXXFLAGS += -g -DDEBUG
debug: $(SHARED_LIB) $(STATIC_LIB)

$(SHARED_LIB): LDFLAGS += -shared
$(SHARED_LIB): $(OBJ)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(STATIC_LIB): $(OBJ)
#	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@
	$(AR) $(ARFLAGS) $(STATIC_LIB) $(OBJ)

clean:
	$(RM) $(SHARED_LIB) $(OBJ) $(DEP) $(STATIC_LIB)

ifeq "$(MAKECMDGOALS)" ""
-include $(DEP)
endif
