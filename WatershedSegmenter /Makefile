INCLUDE_OPENCV = `$(prefix)pkg-config --cflags opencv`
LIBS_OPENCV = `$(prefix)pkg-config --libs opencv`

main:main.cpp WatershedSegmenter.o
	g++ -o main -g -ggdb -O3 $(INCLUDE_OPENCV) main.cpp WatershedSegmenter.o $(LIBS_OPENCV)
WatershedSegmenter.o:WatershedSegmenter.cpp
	g++ -o WatershedSegmenter.o -g -ggdb -c -O3 $(INCLUDE_OPENCV) WatershedSegmenter.cpp $(LIBS_OPENCV)
