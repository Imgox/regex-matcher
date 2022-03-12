#include "utils.h"

t_btree_node *btree_create_node(char c)
{
	t_btree_node *node;

	node = (t_btree_node *)malloc(sizeof(t_btree_node));
	if (node == NULL)
		return (NULL);
	node->c = c;
	node->left = NULL;
	node->right = NULL;
	return (node);
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

char *substring(char *str, int start, int end)
{
	char *sub;
	int i;

	i = 0;
	sub = (char *)malloc(sizeof(char) * (end - start + 2));
	if (sub == NULL)
		return (NULL);
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
	exit(0);
}

int find_closing(char *str, int start)
{
	int i = start;
	int count = 0;

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