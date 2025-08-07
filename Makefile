# Configurable flags
CXX = g++
SRC = biclique_boolean.cpp main.cpp matrix_boolean.cpp multiplicator_boolean.cpp Utils.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = ng

# Define MEMORY and DEBUG via command line if needed
# e.g. make DEBUG=1 MEMORY=1

CXXFLAGS = -std=c++17

ifeq ($(DEBUG),1)
	CXXFLAGS += -g -O0 -DDEBUG=1
else
	CXXFLAGS += -O3 -march=native -flto -funroll-loops -fomit-frame-pointer
endif

ifeq ($(MEMORY),1)
	CXXFLAGS += -DMEMORY
endif

ifeq ($(BITS64),1)
	CXXFLAGS += -DBITS64
endif

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

git:
	git push origin ng --force
