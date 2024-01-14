OBJS	=   	main.o \
				matrix.o \
				multiplicator.o \

SOURCE	=       main.cpp \
				matrix.cpp \
				multiplicator.cpp \

HEADER	=       matrix.hpp \
				multiplicator.hpp \

OUT	=           matrix_multiplicator
CC	 =          g++
FLAGS =         -c  -std=c++20 -I.
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

clean:
	rm -f $(OBJS) $(OBJS_CHECKER) $(OUT)