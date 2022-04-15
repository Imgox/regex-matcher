NAME = mptlc
SRC = utils.c mptlc.c main.c 
CFLAGS = -Wall -Wextra -Werror
INCLUDE = -I ./include
CC = gcc
OBJ = $(SRC:%.c=bin/%.o)
BIN_DIR = bin
RM = rm -rf
MKDIR = mkdir -p

all: $(NAME)

$(NAME): $(BIN_DIR) $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDE) $(OBJ) -o $(NAME)

$(BIN_DIR):
	$(MKDIR) $(BIN_DIR) > /dev/null 2>&1

$(BIN_DIR)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

clean:
	$(RM) $(BIN_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean