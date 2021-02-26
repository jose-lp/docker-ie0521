CC = g++
CFLAGS = -pg -Wall -std=c++11 -O3 -I./alglib/include
SRCS = MultipartiteGraphTracker.cpp PrisSerial.cpp VideoManager.cpp PlayerBlob.cpp Segmentator.cpp MultiPartiteGraph.cpp YamlParser.cpp Timer.cpp Memoirs.cpp BiPartiteTracklerGraph.cpp Dominant.cpp
PROG = MultipartiteGraphTracker

OPENCV = `pkg-config opencv --cflags --libs`
YAMLCPP = `pkg-config yaml-cpp --cflags --libs`
LIBS = $(OPENCV) $(YAMLCPP) -L./ -l:libalglib.a


all: alglib $(PROG) 

$(PROG):$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

alglib:
	cd alglib && $(MAKE)
	cp ./alglib/lib/libalglib.so ./
	cp ./alglib/lib/libalglib.a ./

clean:
	rm -rf $(PROG) *.o lib*.so lib*.a
	rm ./alglib/lib/libalglib*
	rm ./alglib/src/*.o

.PHONY: alglib
