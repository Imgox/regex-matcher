#ifndef BTREE_H
#define BTREE_H

typedef struct s_btree_node
{
	char c;
	struct s_btree_node *left;
	struct s_btree_node *right;
} t_btree_node;

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
t_btree_node *regex_to_btree(char *regex);
void regex_to_btree_shunting_yard(t_btree_stack *btree_stack, t_operator_stack *oeprator_stack, char **regex);
t_btree_stack create_btree_stack();
t_operator_stack create_operator_stack();
void push_btree_stack(t_btree_stack *stack, t_btree_node *el);
t_btree_node *pop_btree_stack(t_btree_stack *stack);
char top_operator_stack(t_operator_stack stack);
void push_operator_stack(t_operator_stack *stack, char el);
int is_op(char c);
char pop_operator_stack(t_operator_stack *stack);

#endif