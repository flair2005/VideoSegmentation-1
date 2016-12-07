INCLUDE_OPENCV = `$(PREFIX)pkg-config --cflags opencv`
LIBS_OPENCV = `$(PREFIX)pkg-config --libs opencv`
DIR_BIN = ./bin
DIR_INC = ./include
DIR_SRC = ./src
DIR_OBJ = ./obj

TARGET = main
BIN_TARGET = $(DIR_BIN)/$(TARGET)
SRC_TARGET = ./$(TARGET).cpp

ISP = ImgSeqProcessor
OBJ_ISP = $(DIR_OBJ)/$(ISP).o
SRC_ISP = $(DIR_SRC)/$(ISP).cpp

MDB = myDirBrowser
OBJ_MDB = $(DIR_OBJ)/$(MDB).o

CFLAGS = -g -ggdb -O3 -I$(DIR_INC)

CC = g++

$(BIN_TARGET):$(SRC_TARGET)
	$(CC) -o $@ $(CFLAGS) $(INCLUDE_OPENCV) $(SRC_TARGET) $(OBJ_ISP) $(OBJ_MDB) $(LIBS_OPENCV)

$(OBJ_ISP):$(SRC_ISP)
	$(CC) -o $(OBJ_ISP) -c $(CFLAGS) $(INCLUDE_OPENCV) $(SRC_ISP) $(OBJ_MDB) $(LIBS_OPENCV)
