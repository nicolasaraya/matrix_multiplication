OBJS	=   	main.o \
				csr_multiplication.o \
				Graph.o \
				GraphWeighted.o \
				Node.o \
				Utils.o

SOURCE	=       main.cpp \
				../../biclique_extraction/Graph/Graph.cpp \
				../../biclique_extraction/Graph/GraphWeighted.cpp \
				../../biclique_extraction/Graph/Node.cpp \
				../../biclique_extraction/Utils/Utils.cpp \
				csr_multiplication.cpp

HEADER	=       ../../biclique_extraction/Graph/Graph.hpp \
				../../biclique_extraction/Graph/GraphWeighted.hpp \
				../../biclique_extraction/Graph/GraphADT.hpp \
				../../biclique_extraction/Graph/Node.hpp \
				../../biclique_extraction/Utils/Utils.hpp \
				csr_multiplication.cpp

OUT	=           csr_multiplicator
CC	 =          g++
FLAGS =         -c  -std=c++20
DEBUG_FLAGS =	-O0 -g -Wall
OPT = 			-O3
LFLAGS	=       -lm
PARALLEL_FLAGS = -lpthread -fopenmp -Dparallel                                                                                                                                      

#SDSL_PATH = 
#BOOST_PATH = -I ~/boost/boost_1_83_0/ -L ~/boost/stage/lib/ 
				

# Si la variable DEBUG está definida, agrega las DEBUG_FLAGS
ifdef debug
FLAGS := $(FLAGS) $(DEBUG_FLAGS)
else
FLAGS += $(OPT) # Si no se define DEBUG, se agregan las banderas de optimización
endif

ifdef paralell
FLAGS := $(FLAGS) $(PARALLEL_FLAGS)
endif



all: $(OBJS)
	$(CC) $(OBJS) -o $(OUT) $(LFLAGS)

main.o: main.cpp
	$(CC) $(FLAGS) main.cpp 

csr.o: csr_multiplication.cpp
	$(CC) $(FLAGS) csr_multiplication.cpp

Graph.o: ../../biclique_extraction/Graph/Graph.cpp
	$(CC) $(FLAGS) ../../biclique_extraction/Graph/Graph.cpp 

GraphWeighted.o: ../../biclique_extraction/Graph/GraphWeighted.cpp
	$(CC) $(FLAGS) ../../biclique_extraction/Graph/GraphWeighted.cpp 

Node.o: ../../biclique_extraction/Graph/Node.cpp
	$(CC) $(FLAGS) ../../biclique_extraction/Graph/Node.cpp 

Utils.o: ../../biclique_extraction/Utils/Utils.cpp
	$(CC) $(FLAGS) ../../biclique_extraction/Utils/Utils.cpp 

sdsl_compressor.o: sdsl_compressor/sdsl_compressor.cpp
	$(CC) $(FLAGS) sdsl_compressor/sdsl_compressor.cpp 

clean:
	rm -f $(OBJS) $(OBJS_CHECKER) $(OUT) $(OUT_CHECKER) sdsl_compressor.o
