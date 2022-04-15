#include "utils.h"

void ft_putchar(char c)
{
	write(1, &c, 1);
}

void insert_star(t_btree_node **tree)
{
	t_btree_node *root;
	char el;

	root = *tree;
	while ((*tree)->right)
		*tree = (*tree)->right;
	el = (*tree)->c;
	(*tree)->c = STAR;
	(*tree)->left = btree_create_node(el);
	*tree = root;
}

t_btree_node *regex_to_btree_elem(t_btree_node *tree, char **regex)
{
	char c = (*regex)[0];
	t_btree_node *res;

	if (!tree)
	{
		if (c == STAR || c == OR || c == CLOSE_PARENTHESIS)
			throw_error(ERR_INV_REGEX);
		else if (c != OPEN_PARENTHESIS)
		{
			(*regex)++;
			return btree_create_node(c);
		}
	}
	if (c == OR)
	{
		int next_or = find_char(*regex, OR, 1);
		res = btree_create_node(OR);
		res->left = tree;
		res->right = regex_to_btree(substring(*regex, 1, next_or));
		*regex = next_or >= 0 ? (*regex) + next_or : 0;
		// btree_apply_postfix(res, &ft_putchar);
		return res;
	}
	if (c == STAR)
	{
		insert_star(&tree);
		(*regex)++;
		return tree;
	}
	if (c == OPEN_PARENTHESIS)
	{
		int end = find_closing(*regex, 1);
		if (end < 0)
			throw_error(ERR_INV_REGEX);
		res = btree_create_node(CONCAT);
		res->left = tree;
		res->right = regex_to_btree(substring(*regex, 1, end));
		(*regex) += end;
		return res;
	}
	if (c == CLOSE_PARENTHESIS)
		throw_error(ERR_INV_REGEX);
	res = btree_create_node(CONCAT);
	res->left = tree;
	res->right = btree_create_node(c);
	(*regex)++;
	return res;
}

t_btree_node *regex_to_btree(char *regex)
{
	t_btree_node *tree;

	tree = NULL;
	while (regex && regex[0])
		tree = regex_to_btree_elem(tree, &regex);

	return (tree);
}

void thompson_elem(t_btree_node *node)
{
	char c = node->c;

	if (c == CONCAT)
	{
		// t_nfa_node *nfa2 = pop_nfa();
		// t_nfa_node *nfa1 = pop_nfa();
		return;
	}
	else if (c == STAR)
	{
		return;
	}
	else if (c == OR)
	{
		return;
	}
	else
	{
		t_nfa_node *nfa1 = nfa_create_node(e_initial_state);
		t_nfa_node *nfa2 = nfa_create_node(e_final_state);

		nfa1->c1 = c;
		nfa1->next1 = nfa2;
		push_nfa(nfa1);
	}
}

void thompson(t_btree_node *tree)
{
	if (tree == NULL)
		return;
	thompson(tree->left);
	thompson(tree->right);
	thompson_elem(tree);
}

int main(int argc, char *argv[])
{
	if (argc != 3)
		throw_error(ERR_USAGE);
	char *regex = argv[1];
	char *word = argv[2];

	t_btree_node *tree;
	tree = regex_to_btree(regex);
	if (!tree)
		throw_error(ERR_EMPTY_TREE);
	btree_apply_postfix(tree, &ft_putchar);
	ft_putchar('\n');
	thompson(tree);
	show_stack();
	free_tree(tree);

	return 0;
}