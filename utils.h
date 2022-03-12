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

typedef struct s_btree_node
{
	char c;
	struct s_btree_node *left;
	struct s_btree_node *right;
} t_btree_node;

t_btree_node *btree_create_node(char c);
void btree_apply_postfix(t_btree_node *root, void (*applyf)(char));
void free_node(t_btree_node *node);
void free_tree(t_btree_node *root);
char *substring(char *str, int start, int end);
void throw_error(char *str);
int find_closing(char *str, int start);

#endif // UTILS_H