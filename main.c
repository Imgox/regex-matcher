#include "utils.h"

void ft_putchar(char c)
{
	write(1, &c, 1);
}

void insert_star(t_btree_node **tree)
{
	t_btree_node *node;
	t_btree_node *root;
	char tmp;

	node = *tree;
	root = *tree;
	while (node->right)
		node = node->right;
	tmp = node->c;
	node->c = STAR;
	node->left = btree_create_node(OR);
	*tree = root;
}

t_btree_node *regex_to_btree(t_btree_node *tree, char *regex)
{
	// printf("regex_to_btree: %s\n", regex);
	t_btree_node *tmp;
	char c = regex[0];

	if (c == '\0')
		return (tree);
	if (!tree)
	{
		if (c == STAR || c == OR || c == CLOSE_PARENTHESIS)
			throw_error("Error: Invalid regex");
		else
			return regex_to_btree(btree_create_node(c), regex + 1);
	}
	if (c == STAR)
	{
		insert_star(&tree);
		btree_apply_postfix(tree, &ft_putchar);
		return regex_to_btree(tree, regex + 1);
	}
	if (c == OR)
	{
		tmp = btree_create_node(OR);
		tmp->left = tree;
		tmp->right = regex_to_btree(NULL, regex + 1);
		return (tmp);
	}
	if (c == OPEN_PARENTHESIS)
	{
		tmp = btree_create_node(CONCAT);
		tmp->left = tree;
		tmp->right = regex_to_btree(NULL, substring(regex, 1, find_closing(regex, 1)));
		return (tmp);
	}
	tmp = btree_create_node(CONCAT);
	tmp->left = tree;
	tmp->right = regex_to_btree(btree_create_node(c), regex + 1);
	return (tmp);
}

int main(int argc, char *argv[])
{
	if (argc != 3)
		throw_error(ERR_USAGE);
	char *regex = argv[1];
	char *word = argv[2];

	t_btree_node *tree = regex_to_btree(NULL, regex);

	btree_apply_postfix(tree, &ft_putchar);

	free_tree(tree);

	return 0;
}