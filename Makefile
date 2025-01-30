
OBJS	= 	main.o \
					matrix.o \
					multiplicator.o \
					biclique.o \
					matrix_boolean.o \
					multiplicator_boolean.o \
					biclique_boolean.o \
					utils.o \
					\
					Shingle.o \
					Trie.o \
					Cluster.o \
					GraphStd.o \
					Graph.o \
					GraphWeighted.o \
					Node.o \
					AttrMgr.o \

SOURCE	=	main.cpp \
					matrix.cpp \
					multiplicator.cpp \
					biclique.cpp \
					boolean/biclique_boolean.cpp \
					boolean/matrix_boolean.cpp \
					boolean/multiplicator_boolean.cpp \
					\
					include/Utils/Utils.cpp \
					include/Graph/Node.cpp \
					include/Graph/Graph.cpp \
					include/Graph/GraphStd.cpp \
					include/Graph/GraphWeighted.cpp \
					include/AttrMgr.cpp \
					include/Cluster.cpp \
					include/Shingle.cpp \
					include/Trie.cpp \

HEADER	=	matrix.hpp \
					multiplicator.hpp \
					biclique.hpp \
					boolean/biclique_boolean.hpp \
					boolean/matrix_boolean.hpp \
					boolean/multiplicator_boolean.hpp \
					\
					include/Graph/BicliqueExtracted.hpp \
					include/Graph/Node.hpp \
					include/Utils/Utils.hpp \
					include/Utils/DebugSystem.hpp \
					include/Graph/Graph.hpp \
					include/Graph/GraphStd.hpp \
					include/Graph/GraphWeighted.hpp \
					include/Cluster.hpp \
					include/Shingle.hpp \
					include/Trie.hpp \
					include/AttrMgr.hpp \

OBJ_FOLD = build

debug_level ?= 0

BE_PATH =  $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))/../biclique_extraction
CUR_PATH =  $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
REPO_URL = https://github.com/nicolasaraya/biclique_extraction.git

OUT	=           matrix_multiplicator
OUT_DEBUG =			matrix_multiplicator-g
CC	 =          g++
FLAGS =         -c  -std=c++17 -I. -Iboolean -Iinclude -Iinclude/Utils -Iinclude/Graph -DDEBUG_LEVEL=$(debug_level)
DEBUG_FLAGS =		-O0 -g -Wall
OPT = 					-Ofast
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

utils.o: include/Utils/Utils.cpp
	$(CC) $(FLAGS) include/Utils/Utils.cpp -o $(OBJ_FOLD)/utils.o

matrix.o: matrix.cpp
	$(CC) $(FLAGS) matrix.cpp -o $(OBJ_FOLD)/matrix.o

multiplicator.o: multiplicator.cpp
	$(CC) $(FLAGS) multiplicator.cpp -o $(OBJ_FOLD)/multiplicator.o

biclique.o: biclique.cpp
	$(CC) $(FLAGS) biclique.cpp -o $(OBJ_FOLD)/biclique.o

biclique_boolean.o: boolean/biclique_boolean.cpp
	$(CC) $(FLAGS) boolean/biclique_boolean.cpp -o $(OBJ_FOLD)/biclique_boolean.o

matrix_boolean.o: boolean/matrix_boolean.cpp 
	$(CC) $(FLAGS) boolean/matrix_boolean.cpp -o $(OBJ_FOLD)/matrix_boolean.o

multiplicator_boolean.o: boolean/multiplicator_boolean.cpp
	$(CC) $(FLAGS) boolean/multiplicator_boolean.cpp -o $(OBJ_FOLD)/multiplicator_boolean.o

AttrMgr.o: include/AttrMgr.cpp
	$(CC) $(FLAGS) include/AttrMgr.cpp -o $(OBJ_FOLD)/AttrMgr.o

Shingle.o: include/Shingle.cpp
	$(CC) $(FLAGS) include/Shingle.cpp -o $(OBJ_FOLD)/Shingle.o

Trie.o: include/Trie.cpp
	$(CC) $(FLAGS) include/Trie.cpp -o $(OBJ_FOLD)/Trie.o

Cluster.o: include/Cluster.cpp
	$(CC) $(FLAGS) include/Cluster.cpp -o $(OBJ_FOLD)/Cluster.o

Graph.o: include/Graph/Graph.cpp
	$(CC) $(FLAGS) include/Graph/Graph.cpp -o $(OBJ_FOLD)/Graph.o
	
GraphStd.o: include/Graph/GraphStd.cpp
	$(CC) $(FLAGS) include/Graph/GraphStd.cpp -o $(OBJ_FOLD)/GraphStd.o

GraphWeighted.o: include/Graph/GraphWeighted.cpp
	$(CC) $(FLAGS) include/Graph/GraphWeighted.cpp -o $(OBJ_FOLD)/GraphWeighted.o

Node.o: include/Graph/Node.cpp
	$(CC) $(FLAGS) include/Graph/Node.cpp -o $(OBJ_FOLD)/Node.o

config:
	@if [ ! -d "$(BE_PATH)" ]; then \
		echo "Biclique Extractor not found, Cloning..."; \
		git clone $(REPO_URL) $(FOLDER_PATH); \
	else \
		echo "Biclique Extractor founded."; \
	fi
	rm -rf ${CUR_PATH}/include
	mkdir -p ${CUR_PATH}/include
	mkdir -p ${CUR_PATH}/include/Graph
	mkdir -p ${CUR_PATH}/include/Utils
	ln -s ${BE_PATH}/Utils/Utils.hpp ${CUR_PATH}/include/Utils/Utils.hpp
	ln -s ${BE_PATH}/Utils/Utils.cpp ${CUR_PATH}/include/Utils/Utils.cpp
	ln -s ${BE_PATH}/Utils/DebugSystem.hpp ${CUR_PATH}/include/Utils/DebugSystem.hpp
	ln -s ${BE_PATH}/Graph/Node.hpp ${CUR_PATH}/include/Graph/Node.hpp
	ln -s ${BE_PATH}/Graph/Node.cpp ${CUR_PATH}/include/Graph/Node.cpp
	ln -s ${BE_PATH}/Graph/BicliqueExtracted.hpp ${CUR_PATH}/include/Graph/BicliqueExtracted.hpp
	ln -s ${BE_PATH}/Graph/Graph.hpp ${CUR_PATH}/include/Graph/Graph.hpp
	ln -s ${BE_PATH}/Graph/Graph.cpp ${CUR_PATH}/include/Graph/Graph.cpp
	ln -s ${BE_PATH}/Graph/GraphStd.hpp ${CUR_PATH}/include/Graph/GraphStd.hpp
	ln -s ${BE_PATH}/Graph/GraphStd.cpp ${CUR_PATH}/include/Graph/GraphStd.cpp
	ln -s ${BE_PATH}/Graph/GraphWeighted.hpp ${CUR_PATH}/include/Graph/GraphWeighted.hpp
	ln -s ${BE_PATH}/Graph/GraphWeighted.cpp ${CUR_PATH}/include/Graph/GraphWeighted.cpp
	ln -s ${BE_PATH}/Cluster.hpp ${CUR_PATH}/include/Cluster.hpp
	ln -s ${BE_PATH}/Cluster.cpp ${CUR_PATH}/include/Cluster.cpp
	ln -s ${BE_PATH}/AttrMgr.hpp ${CUR_PATH}/include/AttrMgr.hpp
	ln -s ${BE_PATH}/AttrMgr.cpp ${CUR_PATH}/include/AttrMgr.cpp
	ln -s ${BE_PATH}/Shingle.hpp ${CUR_PATH}/include/Shingle.hpp
	ln -s ${BE_PATH}/Shingle.cpp ${CUR_PATH}/include/Shingle.cpp
	ln -s ${BE_PATH}/Trie.hpp ${CUR_PATH}/include/Trie.hpp
	ln -s ${BE_PATH}/Trie.cpp ${CUR_PATH}/include/Trie.cpp
	

clean:
	rm -rf $(OBJS) $(OBJS_CHECKER) $(OUT) $(OUT_DEBUG) $(OBJ_FOLD) 

test:
	echo ${BE_PATH}
	echo ${CUR_PATH}