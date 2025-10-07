# Configurable flags
CXX = g++
SRC = biclique_boolean.cpp matrix_boolean.cpp multiplicator_boolean.cpp Utils.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = ng merged pow4 bin2Txt

# Define MEMORY and DEBUG via command line if needed
# e.g. make DEBUG=1 MEMORY=1

CXXFLAGS = -std=c++17

ifeq ($(DEBUGMSG),1)
	CXXFLAGS += -DDEBUG=1
endif

ifeq ($(DEBUG),1)
	CXXFLAGS = -g -O0
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

ng: $(OBJ) main.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

merged: $(OBJ) ng_merged.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

pow4: $(OBJ) ng_pow4.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

bin2Txt: $(OBJ) bin2Txt.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

git:
	git push origin ng --force
