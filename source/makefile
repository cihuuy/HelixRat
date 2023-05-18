# compile project multi file cpp with headers
# makefile for linux

# compiler
CC = g++

# compiler flags
CFLAGS = -Iinclude   -Wall   -std=c++11 -O3 -g

# linker flags
LFLAGS = 

# source files
CPP_SRC := $(wildcard src/*.cpp)
C_SRC := $(wildcard src/*.c)

# object files in ./obj
CPP_OBJ := $(patsubst src/%.cpp, obj/%.o, $(CPP_SRC))
C_OBJ := $(patsubst src/%.c, obj/%.o, $(C_SRC))

ALL_OBJ := $(CPP_OBJ) $(C_OBJ)
ALL_SRC := $(CPP_SRC) $(C_SRC)

# executable name
EXE = bin/helixrat

# build executable

$(EXE): $(ALL_OBJ)
	mkdir -p bin obj
	$(CC) $(CFLAGS) $(ALL_OBJ) -o $(EXE) $(LFLAGS)
# strip --strip-all $(EXE)

# build object files

obj/%.o: src/%.cpp
	mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

obj/%.o: src/%.c
	mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@


# clean object files and executable

clean:
	rm -f $(ALL_OBJ) $(EXE)
