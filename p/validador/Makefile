CC = g++
CFLAGS = -g -Wall -std=c++11 $(shell pkg-config --cflags yaml-cpp) -fopenmp
SRCS = ValidadorRastreo.cpp YamlParser.cpp Measurer.cpp
PROG = ValidadorRastreo

LIBS = $(shell pkg-config --libs yaml-cpp) -lm

$(PROG):$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

clean:
	rm -rf $(PROG) *.o
