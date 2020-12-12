CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++17 -pedantic -I./src
OBJS = \
       src/options/options.o \
       src/cgroups/cgroups.o \
       src/mymoulette.o

all: mymoulette

mymoulette: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

debug: CXXFLAGS += -g3
debug: all

clean:
	$(RM) $(OBJS) mymoulette

.PHONY: clean all debug install
