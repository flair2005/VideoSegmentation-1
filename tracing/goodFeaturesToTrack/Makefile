INCLUDE_OPENCV = `$(prefix)pkg-config --cflags opencv`
LIBS_OPENCV = `$(prefix)pkg-config --libs opencv`

default:main.cpp
	g++ -O3 -ggdb $(INCLUDE_OPENCV) main.cpp $(LIBS_OPENCV) -o main
