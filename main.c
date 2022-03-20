#include "mptlc.h"

int main(int argc, char *argv[])
{
	if (argc != 3)
		throw_error(ERR_USAGE);
	char *regex = argv[1];
	// char *word = argv[2];

	t_btree_node *tree;
	tree = regex_to_btree(regex);
	if (tree)
	{
		btree_apply_postfix(tree, &ft_putchar);
	}
	// btree_apply_postfix(tree, &ft_putchar);
	// free_tree(tree);

	return 0;
}