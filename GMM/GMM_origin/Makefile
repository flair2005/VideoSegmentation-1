LIBS_OPENCV = `$(PREFIX)pkg-config --libs opencv`
INCLUDE_OPENCV = `$(PREFIX)pkg-config --cflags opencv`

default:myMain.cpp
	g++ -o myMain $(INCLUDE_OPENCV) -O3 myMain.cpp $(LIBS_OPENCV)
