CC = gcc
NAME = mptlc

SRC = btree.c state.c nfa.c dfa.c utils.c mptlc.c
SRC_MAIN = $(SRC) main.c
SRC_VERBOSE = $(SRC) verbose.c
SRC_TEST = $(SRC) test.c

INCLUDE = -I ./include

OBJ_MAIN = $(SRC_MAIN:%.c=./bin/%.o)
OBJ_VERBOSE = $(SRC_VERBOSE:%.c=./bin/%.o)
OBJ_TEST = $(SRC_TEST:%.c=./bin/%.o)

BIN_DIR = bin
SRC_DIR = src

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

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
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