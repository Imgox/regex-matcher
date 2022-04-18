#ifndef MPTLC_H
#define MPTLC_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define CONCAT '.'
#define OR '|'
#define STAR '*'
#define OPEN_PARENTHESIS '('
#define CLOSE_PARENTHESIS ')'
#define EPSILON 'e'

#define ERR_USAGE "Usage: ./a.exe <regex> <word>"
#define ERR_INV_REGEX "Error: invalid regex."
#define ERR_EMPTY_TREE "Error: regex could not be parsed."
#define ERR_ALLOC "Error: cannot allocate memory."

#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define MAGENTA "\033[0;35m"
#define CYAN "\033[0;36m"
#define RESET "\033[0m"

typedef struct s_btree_node
{
	char c;
	struct s_btree_node *left;
	struct s_btree_node *right;
} t_btree_node;

typedef struct s_transition
{
	char c;
	struct s_state *next;
} t_transition;

typedef struct s_state
{
	int type;
	t_transition **transitions;
	int transition_count;
} t_state;

typedef struct s_automaton
{
	int type;
	t_state *start;
	t_state **states;
	t_state *end;
	int state_count;
} t_automaton;

typedef enum e_state_type
{
	e_initial_state,
	e_final_state,
	e_intermediate_state,
} t_state_type;

typedef enum e_automaton_type
{
	e_nfa,
	e_dfa
} t_automaton_type;

t_btree_node *create_btree_node(char c);
void btree_apply_postfix(t_btree_node *root, void (*applyf)(char));
void free_btree_node(t_btree_node *node);
void free_btree(t_btree_node *root);
char *substring(char *str, int start, int end);
void throw_error(char *str);
int find_closing(char *str, int start);
int find_next_or(char *str, int start);
t_btree_node *regex_to_btree_elem(t_btree_node *tree, char **regex);
t_btree_node *regex_to_btree(char *regex);
int find_char(char *str, char c, int start);
t_btree_node *copy_btree(t_btree_node *tree);
t_transition *create_transition(char c, t_state *next);
void add_transition(t_state **state, t_transition *transition);
void update_transition(t_transition **transition, char c, t_state *next);
t_state *create_state(t_state_type type);
void add_state(t_automaton **nfa, t_state *state);
void free_state(t_state *state);
void free_states(t_state **states);
t_automaton *create_automaton(t_automaton_type type);
void free_nfa(t_automaton *nfa);
void push_nfa(t_automaton *nfa);
t_automaton *pop_nfa();
t_automaton *top_nfa();
void show_stack();
void thompson_elem(t_btree_node *node);
void thompson(t_btree_node *tree);
t_automaton *create_dfa_from_nfa(t_automaton *nfa);
void ft_putchar(char c);

#endif // UTILS_H