LIBS_OPENCV = `$(PREFIX)pkg-config --libs opencv`
INCLUDE_OPENCV = `$(PREFIX)pkg-config --cflags opencv`


defalt:main.cpp
	g++ -o main -ggdb -g $(INCLUDE_OPENCV) main.cpp $(LIBS_OPENCV)
