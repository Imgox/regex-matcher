#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define CONCAT '.'
#define OR '|'
#define STAR '*'
#define OPEN_PARENTHESIS '('
#define CLOSE_PARENTHESIS ')'
#define ERR_USAGE "Usage: ./a.exe <regex> <word>"
#define ERR_INV_REGEX "Error: invalid regex."
#define ERR_EMPTY_TREE "Error: regex could not be parsed."
#define ERR_ALLOC "Error: cannot allocate memory."
typedef struct s_btree_node
{
	char c;
	struct s_btree_node *left;
	struct s_btree_node *right;
} t_btree_node;

typedef struct s_nfa_node
{
	int type;
	char c1;
	char c2;
	struct s_nfa_node *next1;
	struct s_nfa_node *next2;
} t_nfa_node;

typedef enum e_type
{
	e_initial_state,
	e_final_state,
	e_normal_state,
} t_type;

t_btree_node *btree_create_node(char c);
void btree_apply_postfix(t_btree_node *root, void (*applyf)(char));
void free_node(t_btree_node *node);
void free_tree(t_btree_node *root);
char *substring(char *str, int start, int end);
void throw_error(char *str);
int find_closing(char *str, int start);
t_btree_node *regex_to_btree_elem(t_btree_node *tree, char **regex);
t_btree_node *regex_to_btree(char *regex);
int find_char(char *str, char c, int start);
t_btree_node *btree_copy(t_btree_node *tree);
t_nfa_node *nfa_create_node(int type);
void push_nfa(t_nfa_node *nfa);
t_nfa_node *pop_nfa();
t_nfa_node *top_nfa();
void show_stack();

#endif // UTILS_H