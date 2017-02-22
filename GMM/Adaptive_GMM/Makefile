LIBS_OPENCV = `$(prefix)pkg-config --libs opencv`
INCLUDE_OPENCV = `$(prefix)pkg-config --cflags opencv`
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

CFLAGS = -O3 -ggdb -pedantic -I$(DIR_INC)

CC = g++

$(BIN_TARGET):$(SRC_TARGET) $(OBJ_ISP) $(OBJ_MDB) 
	$(CC) -o $@ $(CFLAGS) $(INCLUDE_OPENCV) $(SRC_TARGET) $(OBJ_ISP) $(OBJ_MDB) $(LIBS_OPENCV)

$(OBJ_ISP):$(SRC_ISP) $(OBJ_MDB)
	$(CC) -o $(OBJ_ISP) -c $(CFLAGS) $(INCLUDE_OPENCV) $(SRC_ISP) $(OBJ_MDB) $(LIBS_OPENCV)
