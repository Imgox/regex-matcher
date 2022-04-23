NAME = mptlc
SRC_MAIN = utils.c mptlc.c main.c
SRC_VERBOSE = utils.c mptlc.c verbose.c
SRC_TEST = utils.c mptlc.c test.c
# CFLAGS = -Wall -Wextra -Werror
INCLUDE = -I ./include
CC = gcc
OBJ_MAIN = $(SRC_MAIN:%.c=bin/%.o)
OBJ_VERBOSE = $(SRC_VERBOSE:%.c=bin/%.o)
OBJ_TEST = $(SRC_TEST:%.c=bin/%.o)
BIN_DIR = bin
RM = rm -rf
MKDIR = mkdir -p

RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[0;33m
RESET = \033[0m

all: $(NAME)

$(NAME): $(BIN_DIR) $(OBJ_MAIN)
	@echo "\n[./$(NAME)] ${YELLOW}Making..${RESET}"
	@$(CC) $(CFLAGS) $(INCLUDE) $(OBJ_MAIN) -o $(NAME)
	@echo "[./$(NAME)] ${GREEN}Made!\n${RESET}"

verbose: $(BIN_DIR) $(OBJ_VERBOSE)
	@echo "\n[./$(NAME)] ${YELLOW}Making..${RESET}"
	@$(CC) $(CFLAGS) $(INCLUDE) $(OBJ_VERBOSE) -o $(NAME)
	@echo "[./$(NAME)] ${GREEN}Made!\n${RESET}"

test: $(BIN_DIR) $(OBJ_TEST)
	@echo "\n[./$(NAME)] ${YELLOW}Making..${RESET}"
	@$(CC) $(CFLAGS) $(INCLUDE) $(OBJ_TEST) -o $(NAME)
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
re_test: fclean test
re_verbose: fclean verbose

.PHONY: all clean fclean