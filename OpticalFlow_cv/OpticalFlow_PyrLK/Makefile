LIBS_OPENCV = `$(PREFIX)pkg-config --libs opencv`
INCLUDE_OPENCV = `$(PREFIX)pkg-config --cflags opencv`

default:myMain.cpp
	g++ -o myMain -O3 $(INCLUDE_OPENCV) myMain.cpp -ggdb $(LIBS_OPENCV)
