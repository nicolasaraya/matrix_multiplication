
OBJS	= 	main.o \
					matrix.o \
					multiplicator.o \
					utils.o \
					biclique.o \
					matrix_boolean.o \
					multiplicator_boolean.o \
					biclique_boolean.o \

SOURCE	=	main.cpp \
					matrix.cpp \
					multiplicator.cpp \
					utils.cpp \
					biclique.cpp \
					boolean/biclique_boolean.cpp \
					boolean/matrix_boolean.cpp \
					boolean/multiplicator_boolean.cpp \
					\
					../biclique_extraction/Utils/Utils.cpp \
					../biclique_extraction/Graph/Node.cpp \

HEADER	=	matrix.hpp \
					multiplicator.hpp \
					utils.hpp \
					biclique.hpp \
					boolean/biclique_boolean.hpp \
					boolean/matrix_boolean.hpp \
					boolean/multiplicator_boolean.hpp \
					\
					../biclique_extraction/Utils/Utils.hpp \
					../biclique_extraction/Graph/Biclique.hpp \
					../biclique_extraction/Graph/Node.hpp \

OBJ_FOLD = build

debug_level ?= 0

BE_PATH = ../biclique_extraction
REPO_URL = https://github.com/nicolasaraya/biclique_extraction.git

OUT	=           matrix_multiplicator
OUT_DEBUG =			matrix_multiplicator-g
CC	 =          g++
FLAGS =         -c  -std=c++20 -I. -Iboolean -I${BE_PATH} -I${BE_PATH}/Graph -I${BE_PATH}/Utils -DDEBUG_LEVEL=$(debug_level)
DEBUG_FLAGS =		-O0 -g -Wall
OPT = 					-O3
LFLAGS	=       -lm

# Si la variable DEBUG está definida, agrega las DEBUG_FLAGS
ifdef debug
FLAGS += $(DEBUG_FLAGS)
else
FLAGS += $(OPT) # Si no se define DEBUG, se agregan las banderas de optimización
endif


# Objetivo para verificar y clonar la carpeta si no existe

all: 
	mkdir -p $(OBJ_FOLD)
	make $(OBJS)
	$(CC) $(OBJ_FOLD)/* -o $(OUT) $(LFLAGS)

debug:
	mkdir -p $(OBJ_FOLD)
	make debug=1 $(OBJS)
	$(CC) $(DEBUG_FLAGS) $(OBJ_FOLD)/* -o $(OUT_DEBUG) $(LFLAGS)

main.o: main.cpp
	$(CC) $(FLAGS) main.cpp -o $(OBJ_FOLD)/main.o

matrix.o: matrix.cpp
	$(CC) $(FLAGS) matrix.cpp -o $(OBJ_FOLD)/matrix.o

multiplicator.o: multiplicator.cpp
	$(CC) $(FLAGS) multiplicator.cpp -o $(OBJ_FOLD)/multiplicator.o

utils.o: utils.cpp
	$(CC) $(FLAGS) utils.cpp -o $(OBJ_FOLD)/utils.o

biclique.o: biclique.cpp
	$(CC) $(FLAGS) biclique.cpp -o $(OBJ_FOLD)/biclique.o

biclique_boolean.o: boolean/biclique_boolean.cpp
	$(CC) $(FLAGS) boolean/biclique_boolean.cpp -o $(OBJ_FOLD)/biclique_boolean.o

matrix_boolean.o: boolean/matrix_boolean.cpp 
	$(CC) $(FLAGS) boolean/matrix_boolean.cpp -o $(OBJ_FOLD)/matrix_boolean.o

multiplicator_boolean.o: boolean/multiplicator_boolean.cpp
	$(CC) $(FLAGS) boolean/multiplicator_boolean.cpp -o $(OBJ_FOLD)/multiplicator_boolean.o

config:
	@if [ ! -d "$(BE_PATH)" ]; then \
		echo "Biclique Extractor not found, Cloning..."; \
		git clone $(REPO_URL) $(FOLDER_PATH); \
	else \
		echo "Biclique Extractor founded."; \
	fi

clean:
	rm -rf $(OBJS) $(OBJS_CHECKER) $(OUT) $(OUT_DEBUG) $(OBJ_FOLD) 