GCC = gcc
INCLUDE = -Iinclude
LINKS = 

CFLAGS = --std=gnu99 -O2

# Debug flags
# CFLAGS = $(CFLAGS) -g -Wall -Wextra -Wshadow -Wcast-align -fsanitize={address,undefined}

INC = $(shell find include/ -type f -name '*.h')
SRC = $(shell find src/ -type f -name '*.c' ! -name 'main.c')
OBJ = $(patsubst src/%.c, build/obj/%.o, $(SRC))
BIN = $(patsubst src/%.c, build/bin/%, src/main.c)

TEST_SRC = $(shell find test/ -type f -name '*.c')
TEST_BIN = $(patsubst test/%.c, build/bin/%, $(TEST_SRC))

all: build $(BIN) $(TEST_BIN)

build: $(OBJ)

build/bin/main: src/main.c $(OBJ) 
	@mkdir -p "$(@D)"
	@echo Compiling "$<"
	@$(GCC) $(CFLAGS) "$<" $(OBJ) -o "$@" $(INCLUDE) $(LINKS)

build/bin/%: test/%.c $(OBJ)
	@mkdir -p "$(@D)"
	@echo Compiling "$<"
	@$(GCC) $(CFLAGS) "$<" $(OBJ) -o "$@" $(INCLUDE) $(LINKS)

build/obj/%.o: src/%.c
	@mkdir -p "$(@D)"
	@echo Compiling "$<"
	@$(GCC) $(CFLAGS) -c "$<" -o "$@" $(INCLUDE) $(LINKS)

clear:
	@rm -rf build

