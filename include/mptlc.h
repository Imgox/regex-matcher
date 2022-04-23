#ifndef MPTLC_H
#define MPTLC_H

#include <stdlib.h>
#include <stdio.h>
// #include <unistd.h>
#include <string.h>

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

typedef enum e_state_type
{
	e_initial_state,
	e_final_state,
	e_intermediate_state,
} t_state_type;

typedef enum e_nfa_type
{
	e_nfa,
	e_dfa
} t_nfa_type;

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
	t_state_type type;
	t_transition **transitions;
	int transition_count;
} t_state;

typedef struct s_state_group
{
	t_state_type type;
	t_state **states;
	t_state *state_eq;
	int state_count;
} t_state_group;

typedef struct s_nfa
{
	t_state *start;
	t_state **states;
	t_state *end;
	int state_count;
} t_nfa;

typedef struct s_dfa
{
	t_state *start;
	t_state **states;
	t_state **ends;
	int state_count;
	int end_count;
} t_dfa;

typedef struct s_btree_stack
{
	t_btree_node **content;
	int size;
} t_btree_stack;

typedef struct s_operator_stack
{
	char *content;
	int size;
} t_operator_stack;

t_btree_node *create_btree_node(char c);
void btree_apply_postfix(t_btree_node *root, void (*applyf)(char));
void free_btree_node(t_btree_node *node);
void free_btree(t_btree_node *root);
char *substring(char *str, int start);
void throw_error(char *str);
int find_closing(char *str);
void regex_to_btree_shunting_yard(t_btree_stack *btree_stack, t_operator_stack *oeprator_stack, char **regex);
t_btree_node *regex_to_btree(char *regex);
int find_char(char *str, char c, int start);
t_btree_node *copy_btree(t_btree_node *tree);
t_transition *create_transition(char c, t_state *next);
void add_transition(t_state **state, t_transition *transition);
void update_transition(t_transition **transition, char c, t_state *next);
t_state *create_state(t_state_type type);
void add_state_to_nfa(t_nfa **nfa, t_state *state);
void add_state_to_dfa(t_dfa **dfa, t_state *state);
void free_state(t_state *state);
void free_states(t_state **states);
t_nfa *create_nfa();
void free_nfa(t_nfa *nfa);
void push_nfa(t_nfa *nfa);
t_nfa *pop_nfa();
t_nfa *top_nfa();
void show_stack();
void print_dfa(t_dfa *dfa);
void thompson_elem(t_btree_node *node);
void thompson(t_btree_node *tree);
t_dfa *create_dfa();
t_dfa *create_dfa_from_nfa(t_nfa *nfa);
t_state_group *create_group(t_state_type type);
t_state *is_group_in_groups(t_state_group **groups, int group_count, t_state_group *group);
int is_state_in_group(t_state_group *group, t_state *state);
void add_state_to_group(t_state_group **group, t_state *state);
void print_group(t_state_group *group, int index);
void print_groups(t_state_group **groups, int group_count);
int contains_only_epsilon_transitions(t_state *state);
int recognize(t_state *current_state, char *word);
t_btree_stack create_btree_stack();
t_operator_stack create_operator_stack();
void push_btree_stack(t_btree_stack *stack, t_btree_node *el);
t_btree_node *pop_btree_stack(t_btree_stack *stack);
char top_operator_stack(t_operator_stack stack);
void push_operator_stack(t_operator_stack *stack, char el);
char pop_operator_stack(t_operator_stack *stack);
int is_op(char c);
char *insert_concat(const char *regex);
char *insert_paranthesis(const char *regex);
void ft_putchar(char c);

#endif // UTILS_H