CC = g++
CFLAGS = -g -Wall -std=c++11
SRCS = VideoJoiner.cpp VideoManager.cpp YamlParser.cpp Joiner.cpp cv.cpp ImageManager.cpp JGpu.cpp
PROG = VideoJoiner

OPENCV = `pkg-config opencv --cflags --libs`
YAMLCPP = `pkg-config yaml-cpp --cflags --libs`
GTK = `pkg-config --cflags --libs gtk+-2.0`
LIBS = $(OPENCV) $(YAMLCPP) -lboost_system -lboost_filesystem -L$(BOOSTLIB)

$(PROG):$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

clean:
	rm -rf $(PROG) *.o
