LIBS_OPENCV = `$(PREFIX)pkg-config --libs opencv`
INCLUDE_OPENCV = `$(PREFIX)pkg-config --cflags opencv`
DIR_INC = ./include
DIR_OBJ = ./obj
DIR_SRC = ./src
DIR_BIN = ./bin

TARG_NAME = main
TARG_PATH = $(DIR_BIN)/$(TARG_NAME)
TARG_SRC = ./$(TARG_NAME).cpp

FT_NAME = FourierTransform
FT_SRC = $(DIR_SRC)/$(FT_NAME).cpp
FT_OBJ = $(DIR_OBJ)/$(FT_NAME).o

ISC_NAME = ImgSequenceCapture
ISC_SRC = $(DIR_SRC)/$(ISC_NAME).cpp
ISC_OBJ = $(DIR_OBJ)/$(ISC_NAME).o

DB_OBJ = $(DIR_OBJ)/myDirBrowser.o

CFLAGS = -g -ggdb -Wall -I$(DIR_INC)

CC = g++

#ALL:
#	@echo $(TARG_PATH), $(TARG_SRC)
#	@echo $(wildcard ${DIR_OBJ}/*.o)

#一定要注意库之间的依赖关系
$(TARG_PATH):$(TARG_SRC) $(ISC_OBJ) $(DB_OBJ) $(FT_OBJ)
	$(CC) -o $@ $(INCLUDE_OPENCV) $(CFLAGS) $(TARG_SRC) $(ISC_OBJ) $(DB_OBJ) $(FT_OBJ) $(LIBS_OPENCV) 

$(ISC_OBJ):$(ISC_SRC) $(DB_OBJ) $(FT_OBJ)
	$(CC) -o $@ -c $(INCLUDE_OPENCV) $(CFLAGS) $(ISC_SRC) $(DB_OBJ) $(FT_OBJ) $(LIBS_OPENCV)

$(FT_OBJ):$(FT_SRC)
	$(CC) -o $@ -c $(INCLUDE_OPENCV) $(CFLAGS) $(FT_SRC) $(LIBS_OPENCV)



