CC = g++
CFLAGS = -g -Wall -std=c++11
SRCS = AnotarRastreo.cpp VideoManager.cpp YamlParser.cpp
PROG = AnotarRastreo

OPENCV = `pkg-config opencv --cflags --libs`
YAMLCPP = `pkg-config yaml-cpp --cflags --libs`
GTK = `pkg-config --cflags --libs gtk+-2.0`
LIBS = $(OPENCV) $(YAMLCPP) $(GTK)

$(PROG):$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

clean:
	rm -rf $(PROG) *.o
