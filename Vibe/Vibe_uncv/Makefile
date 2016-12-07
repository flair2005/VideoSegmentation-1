LIBS_OPENCV = `$(PREFIX)pkg-config --libs opencv`
INCLUDE_OPENCV = `$(PREFIX)pkg-config --cflags opencv`

myMain:myMain.cpp ViBe.o
	g++ -o myMain -O3 myMain.cpp $(INCLUDE_OPENCV) ViBe.o $(LIBS_OPENCV) -ggdb

Vibe.o:Vibe.cpp
	g++ -o ViBe.o -O3 ViBe.h ViBe.cpp $(INCLUDE_OPENCV) $(LIBS_OPENCV) -ggdb
