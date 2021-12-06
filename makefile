# silent echo calls
# MAKEFLAGS += --silent
# instead silencing individual calls with '@'


# compiler plus flags, -g: debug info, -Wall: all warnings
CC = @gcc -g -Wall

NAME = cow.exe

SRC_DIR = src
INC_DIR = $(SRC_DIR)/include

OBJ_DIR = bin

_OBJS =						\
	main.o					\
	file_io.o				\
	token.o					\
	interpreter.o		

# put the OBJ_DIR in front of the obj names
OBJS = $(patsubst %,$(OBJ_DIR)/%,$(_OBJS))
OBJS_WIN = $(patsubst %,$(OBJ_DIR)\\%,$(_OBJS))


all: $(NAME)
	@echo ---- done ----

# compile all object files to .exe
$(NAME): $(OBJS)
	$(CC) -o $(NAME) $(OBJS)


# compile all .c files to .o files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	echo $< -> $@
	$(CC) -c $< -o $@ -I $(INC_DIR)


# prevents make from fideling with files named clean
.PHONY: clean

clean:
	del $(NAME)
	del $(OBJS_WIN) 
	@echo ---- clean done ---- 

cleanm: clean $(NAME)

