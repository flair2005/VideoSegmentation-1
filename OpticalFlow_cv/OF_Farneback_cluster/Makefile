LIBS_OPENCV = `$(PREFIX)pkg-config --libs opencv`
INCLUDE_OPENCV = `$(PREFIX)pkg-config --cflags opencv`
VLROOT = /home/yangzheng/myLib/vlfeat-0.9.20
LIBS_VLFEAT = $(VLROOT)/bin/glnxa64

default:myMain.cpp
	g++ -o myMain -O3 $(INCLUDE_OPENCV) -I$(VLROOT) myMain.cpp -ggdb $(LIBS_OPENCV) -L$(LIBS_VLFEAT) -lvl
