OBJS	=   	main.o \
				matrix_boolean.o \
				multiplicator_boolean.o \
				utils.o \
				biclique_boolean.o \

SOURCE	=       main.cpp \
				matrix_boolean.cpp \
				multiplicator_boolean.cpp \
				utils.cpp \
				biclique_boolean.cpp \

HEADER	=       matrix_boolean.hpp \
				multiplicator_boolean.hpp \
				utils.hpp \
				biclique_boolean.hpp \

OUT	=           matrix_multiplicator
CC	 =          g++
FLAGS =         -c  -std=c++17 -I. -I..
DEBUG_FLAGS =	-O0 -g -Wall
OPT = 			-Ofast
LFLAGS	=      # -lm                                                                                                                                    

				

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

matrix_boolean.o: matrix_boolean.cpp
	$(CC) $(FLAGS) matrix_boolean.cpp

multiplicator_boolean.o: multiplicator_boolean.cpp
	$(CC) $(FLAGS) multiplicator_boolean.cpp

utils.o: ../utils.cpp
	$(CC) $(FLAGS) ../utils.cpp

biclique_boolean.o: biclique_boolean.cpp
	$(CC) $(FLAGS) biclique_boolean.cpp
clean:
	rm -f $(OBJS) $(OBJS_CHECKER) $(OUT)