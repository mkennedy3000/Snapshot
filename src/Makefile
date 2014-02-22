compiler=gcc
flags=-g -Wall

OBJS = networking.o snapshot.o
OBJ_DIR = obj
OBJ = $(patsubst %,$(OBJ_DIR)/%,$(OBJS))

all: run

run: $(OBJ)
	$(compiler) $^ -o $@

$(OBJ_DIR)/%.o : %.c networking.h
	$(compiler) $(flags) -c $< -o $@

clean:
	rm -rf obj/*.o run

