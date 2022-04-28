#ifndef CONSTANTS_H
#define CONSTANTS_H

#define CONCAT '.'
#define OR '|'
#define STAR '*'
#define OPEN_PARENTHESIS '('
#define CLOSE_PARENTHESIS ')'
#define EPSILON 'e'

#define ERR_USAGE "Usage: ./mptlc <regex> <word>"
#define ERR_INV_REGEX "Error: invalid regex."
#define ERR_EMPTY_TREE "Error: regex could not be parsed."
#define ERR_ALLOC "Error: cannot allocate memory."
#define ERR_TESTS_NOT_FOUND "Error: no tests found."

#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define MAGENTA "\033[0;35m"
#define CYAN "\033[0;36m"
#define RESET "\033[0m"

#define TYPES \
	(char *[]) { "Initial", "Final", "Intermediate" }
#define COLORS \
	(char *[]) { GREEN, RED, YELLOW }

#define A 'a'
#define B 'b'

#endif