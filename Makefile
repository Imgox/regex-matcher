NAME = mptlc
SRC = utils.c mptlc.c main.c 
# CFLAGS = -Wall -Wextra -Werror
INCLUDE = -I ./include
CC = gcc
OBJ = $(SRC:%.c=bin/%.o)
BIN_DIR = bin
RM = rm -rf
MKDIR = mkdir -p

RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[0;33m
RESET = \033[0m

all: $(NAME)

$(NAME): $(BIN_DIR) $(OBJ)
	@echo "\n[./$(NAME)] ${YELLOW}Making..${RESET}"
	@$(CC) $(CFLAGS) $(INCLUDE) $(OBJ) -o $(NAME)
	@echo "[./$(NAME)] ${GREEN}Made!\n${RESET}"

$(BIN_DIR):
	@$(MKDIR) $(BIN_DIR) > /dev/null 2>&1

$(BIN_DIR)/%.o: %.c
	@echo "[./$<] ${YELLOW}Compiling..${RESET}"
	@$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<
	@echo "[./$<] ${GREEN}Compiled!${RESET}"

clean:
	@$(RM) $(BIN_DIR)
	@echo "[./$(BIN_DIR)/] ${RED}Cleaned!${RESET}"

fclean: clean
	@$(RM) $(NAME)
	@echo "[./$(NAME)] ${RED}Cleaned!${RESET}"

re: fclean all

.PHONY: all clean fclean