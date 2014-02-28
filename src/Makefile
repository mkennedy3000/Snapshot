compiler=gcc
flags=-g -Wall

OBJS = networking.o snapshot.o file_io.o
OBJ_DIR = obj
OBJ = $(patsubst %,$(OBJ_DIR)/%,$(OBJS))
REC_DIR = records

all: run directories

run: $(OBJ)
	$(compiler) $^ -o $@ -pthread

$(OBJ_DIR)/%.o : %.c networking.h file_io.h
	$(compiler) $(flags) -c $< -o $@

directories:
	@mkdir -p $(REC_DIR)
	@mkdir -p $(OBJ_DIR)

clean:
	rm -rf run $(OBJ_DIR)/*.o $(REC_DIR)/*
