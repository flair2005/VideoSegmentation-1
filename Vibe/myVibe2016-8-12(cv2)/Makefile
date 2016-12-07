LIBS_OPENCV = `$(PREFIX)pkg-config --libs opencv`
INCLUDE_OPENCV = `$(PREFIX)pkg-config --cflags opencv`

myMain: myMain.cpp myVibe.o
	g++ -O3 -Wall -Werror -pedantic $(INCLUDE_OPENCV) myMain.cpp myVibe.o \
		vibe-background-sequential.o -ggdb $(LIBS_OPENCV) -o myMain

myVibe.o: myVibe.cpp vibe-background-sequential.o
	g++ -O3 -Wall -Werror -pedantic $(INCLUDE_OPENCV) myVibe.cpp vibe-background-sequential.o \
		-c -ggdb $(LIBS_OPENCV)  -o myVibe.o

vibe-background-sequential.o: vibe-background-sequential.c
	gcc -std=c99 -O3  -Wall -Werror -pedantic -Wno-unused-function \
		-Wno-unused-parameter -Wno-deprecated -Wno-deprecated-declarations -Wno-si \
		-c -ggdb vibe-background-sequential.c -o vibe-background-sequential.o

#default:
#	gcc -std=c99 -O3  -Wall -Werror -pedantic -Wno-unused-function -Wno-unused-parameter -Wno-deprecated -Wno-deprecated-declarations -Wno-sign-compare -Wno-unused-but-set-parameter -c -ggdb vibe-background-sequential.c
#	g++ -o main-opencv -O3 -Wall -Werror -pedantic $(INCLUDE_OPENCV) main-opencv.cpp -ggdb vibe-background-sequential.o $(LIBS_OPENCV)
