LIBS_OPENCV = `$(PREFIX)pkg-config --libs opencv`
INCLUDE_OPENCV = `$(PREFIX)pkg-config --cflags opencv`
DIR_INC = ./include
DIR_OBJ = ./obj
DIR_SRC = ./src
DIR_BIN = ./bin

TARG_NAME = main
TARG_PATH = $(DIR_BIN)/$(TARG_NAME)
TARG_SRC = ./$(TARG_NAME).cpp

LIBS_OBJ = $(wildcard ${DIR_OBJ}/*.o)

CFLAGS = -g -ggdb -Wall -I$(DIR_INC)

CC = g++

#ALL:
#	@echo $(TARG_PATH), $(TARG_SRC)
#	@echo $(wildcard ${DIR_OBJ}/*.o)

#一定要注意库之间的依赖关系
$(TARG_PATH):$(TARG_SRC) $(LIBS_OBJ)
	$(CC) -o $@ $(INCLUDE_OPENCV) $(CFLAGS) $(TARG_SRC) $(LIBS_OBJ) $(LIBS_OPENCV) 

$(DIR_OBJ)/ImgSequenceCapture.o:$(DIR_SRC)/ImgSequenceCapture.cpp $(DIR_OBJ)/myDirBrowser.o
	$(CC) -o $@ -c $(INCLUDE_OPENCV) $(CFLAGS) $(LIBS_OPENCV) $(DIR_OBJ)/myDirBrowser.o $(DIR_SRC)/ImgSequenceCapture.cpp



