#include "mptlc.h"

t_nfa_node **g_nfa_stack;
int g_nfa_count = 0;

t_btree_node *btree_create_node(char c)
{
	t_btree_node *node;

	node = (t_btree_node *)malloc(sizeof(t_btree_node));
	if (node == NULL)
		throw_error(ERR_ALLOC);
	node->c = c;
	node->left = NULL;
	node->right = NULL;
	return (node);
}

t_nfa_node *nfa_create_node(int type)
{
	t_nfa_node *node;

	node = (t_nfa_node *)malloc(sizeof(t_nfa_node));
	if (node == NULL)
		throw_error(ERR_ALLOC);
	node->type = type;
	node->c1 = '\0';
	node->c2 = '\0';
	node->next1 = NULL;
	node->next2 = NULL;
	return node;
}

void btree_apply_postfix(t_btree_node *root, void (*applyf)(char))
{
	if (root == NULL)
		return;
	btree_apply_postfix(root->left, applyf);
	btree_apply_postfix(root->right, applyf);
	applyf(root->c);
}

void free_node(t_btree_node *node)
{
	if (node == NULL)
		return;
	free_node(node->left);
	free_node(node->right);
	free(node);
}

void free_tree(t_btree_node *root)
{
	if (root == NULL)
		return;
	free_tree(root->left);
	free_tree(root->right);
	free(root);
}

t_btree_node *btree_copy(t_btree_node *tree)
{
	t_btree_node *copy;

	if (tree == NULL)
		return NULL;

	copy = btree_create_node(tree->c);
	copy->left = btree_copy(tree->left);
	copy->right = btree_copy(tree->right);

	return copy;
}

char *substring(char *str, int start, int end)
{
	char *sub;
	int i;

	i = 0;
	sub = (char *)malloc(sizeof(char) * (end - start + 2));
	if (sub == NULL)
		throw_error(ERR_ALLOC);
	end = end >= 0 ? end : strlen(str);
	while (start < end)
	{
		sub[i] = str[start];
		start++;
		i++;
	}
	sub[i] = '\0';
	return (sub);
}

void throw_error(char *str)
{
	fprintf(stderr, "%s\n", str);
	exit(1);
}

int find_closing(char *str, int start)
{
	int i = start;
	int count = 1;

	while (str[i])
	{
		if (str[i] == OPEN_PARENTHESIS)
			count++;
		if (str[i] == CLOSE_PARENTHESIS)
			count--;
		if (count == 0)
			return (i);
		i++;
	}
	return (-1);
}

int find_char(char *str, char c, int start)
{
	int i = start;
	while (str[i])
	{
		if (str[i] == c)
			return i;
		i++;
	}
	return -1;
}

void push_nfa(t_nfa_node *nfa)
{
	printf("here\n");
	g_nfa_stack = (t_nfa_node **)realloc(g_nfa_stack, (g_nfa_count + 1) * sizeof(t_nfa_node *));
	if (g_nfa_stack == NULL)
		throw_error(ERR_ALLOC);
	g_nfa_stack[g_nfa_count] = nfa;
	g_nfa_count++;
}

t_nfa_node *pop_nfa()
{
	g_nfa_count--;
	t_nfa_node *nfa = g_nfa_stack[g_nfa_count];
	if (g_nfa_stack[g_nfa_count]->next1)
		free(g_nfa_stack[g_nfa_count]->next1);
	if (g_nfa_stack[g_nfa_count]->next2)
		free(g_nfa_stack[g_nfa_count]->next2);
	free(g_nfa_stack[g_nfa_count]);
	g_nfa_stack = realloc(g_nfa_stack, g_nfa_count * sizeof(t_nfa_node *));
	return nfa;
}

t_nfa_node *top_nfa()
{
	return (g_nfa_stack[g_nfa_count - 1]);
}

void print_nfa(t_nfa_node *nfa)
{
	char *types[] = {
		"Initial",
		"Final",
		"Normal",
	};
	if (nfa == NULL)
		return;
	printf("--------------\n");
	printf("State: %s\n", types[nfa->type]);
	printf("--------------\n");
	if (nfa->c1)
		printf("with '%c' go to ->\n", nfa->c1);
	print_nfa(nfa->next1);
	if (nfa->c2)
		printf("with '%c' go to ->\n", nfa->c2);
	print_nfa(nfa->next2);
}

void show_stack()
{
	for (int i = g_nfa_count - 1; i >= 0; i--)
	{
		printf("=============================\n");
		printf("   NFA No %d\n", g_nfa_count - 1 - i);
		printf("=============================\n");
		print_nfa(g_nfa_stack[i]);
		printf("=============================\n");
	}
}

void ft_putchar(char c)
{
	write(1, &c, 1);
}