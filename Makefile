OBJS	=   	main.o \
				matrix.o \
				multiplicator.o \
				utils.o \
				biclique.o \

SOURCE	=       main.cpp \
				matrix.cpp \
				multiplicator.cpp \
				utils.cpp \
				biclique.cpp \

HEADER	=       matrix.hpp \
				multiplicator.hpp \
				utils.hpp \
				biclique.hpp \

OUT	=           matrix_multiplicator
CC	 =          g++-11
FLAGS =         -c  -std=c++17 -I.
DEBUG_FLAGS =	-O0 -g -Wall
OPT = 			-O3
LFLAGS	=       -lm                                                                                                                                    

				

# Si la variable DEBUG está definida, agrega las DEBUG_FLAGS
ifdef debug
FLAGS := $(FLAGS) $(DEBUG_FLAGS)
else
FLAGS += $(OPT) # Si no se define DEBUG, se agregan las banderas de optimización
endif

all: $(OBJS)
	$(CC) $(OBJS) -o $(OUT) $(LFLAGS)

main.o: main.cpp
	$(CC) $(FLAGS) main.cpp 

matrix.o: matrix.cpp
	$(CC) $(FLAGS) matrix.cpp

multiplicator.o: multiplicator.cpp
	$(CC) $(FLAGS) multiplicator.cpp

utils.o: utils.cpp
	$(CC) $(FLAGS) utils.cpp

biclique.o: biclique.cpp
	$(CC) $(FLAGS) biclique.cpp
clean:
	rm -f $(OBJS) $(OBJS_CHECKER) $(OUT)