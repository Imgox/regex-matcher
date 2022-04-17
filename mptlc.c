#include "mptlc.h"

void insert_star(t_btree_node **tree)
{
	t_btree_node *root;
	char el;

	root = *tree;
	while ((*tree)->right)
		*tree = (*tree)->right;
	el = (*tree)->c;
	(*tree)->c = STAR;
	(*tree)->left = create_btree_node(el);
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
			return create_btree_node(c);
		}
	}
	if (c == OR)
	{
		int next_or = find_next_or(*regex, 1);
		res = create_btree_node(OR);
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
		printf("%s -- %d\n", *regex, end);
		if (end < 0)
			throw_error(ERR_INV_REGEX);
		if (tree)
		{
			res = create_btree_node(CONCAT);
			res->left = tree;
			res->right = regex_to_btree(substring(*regex, 1, end));
		}
		else
		{
			res = regex_to_btree(substring(*regex, 1, end));
		}
		(*regex) += end;
		return res;
	}
	if (c == CLOSE_PARENTHESIS)
	{
		// throw_error(ERR_INV_REGEX);
		(*regex)++;
		return tree;
	}
	res = create_btree_node(CONCAT);
	res->left = tree;
	res->right = create_btree_node(c);
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
		t_nfa *nfa2 = pop_nfa();
		t_nfa *nfa1 = pop_nfa();
		t_nfa *nfa = create_nfa();
		int i = 0;
		while (i < nfa1->state_count)
		{
			if (i == 0)
				nfa->start = nfa1->states[i];
			t_state *s = nfa1->states[i];
			if (s->type != e_final_state)
				add_state(&nfa, s);
			int j = 0;
			while (j < s->transition_count)
			{
				t_transition *t = s->transitions[j];
				if (t->next->type == e_final_state)
				{
					update_transition(&t, t->c, nfa2->start);
				}
				j++;
			}
			i++;
		}
		i = 0;
		while (i < nfa2->state_count)
		{
			t_state *s = nfa2->states[i];
			if (s->type == e_initial_state)
				s->type = e_intermediate_state;
			add_state(&nfa, s);
			i++;
		}
		nfa->end = nfa2->states[i - 1];
		push_nfa(nfa);
		return;
	}
	else if (c == STAR)
	{
		t_nfa *nfa1 = pop_nfa();
		t_nfa *nfa = create_nfa();
		t_state *start = create_state(e_initial_state);
		t_state *end = create_state(e_final_state);
		t_transition *t = create_transition(EPSILON, nfa1->start);
		add_transition(&start, t);
		t = create_transition(EPSILON, end);
		add_transition(&start, t);
		add_state(&nfa, start);
		add_state(&nfa, end);
		nfa->start = start;
		nfa->end = end;
		int i = 0;
		while (nfa1->states && i < nfa1->state_count)
		{
			t_state *s = nfa1->states[i];
			if (s->type == e_final_state)
			{
				s->type = e_intermediate_state;
				t_transition *t = create_transition(EPSILON, end);
				add_transition(&s, t);
				t = create_transition(EPSILON, nfa1->start);
				add_transition(&s, t);
			}
			if (s->type == e_initial_state)
				s->type = e_intermediate_state;
			add_state(&nfa, s);
			i++;
		}
		push_nfa(nfa);
		return;
	}
	else if (c == OR)
	{
		t_nfa *nfa2 = pop_nfa();
		t_nfa *nfa1 = pop_nfa();
		t_nfa *nfa = create_nfa();
		t_state *start = create_state(e_initial_state);
		t_state *end = create_state(e_final_state);
		t_transition *t = create_transition(EPSILON, nfa1->start);
		add_transition(&start, t);
		t = create_transition(EPSILON, nfa2->start);
		add_transition(&start, t);
		add_state(&nfa, start);
		add_state(&nfa, end);
		nfa->start = start;
		nfa->end = end;
		int i = 0;
		while (nfa1->states && i < nfa1->state_count)
		{
			t_state *s = nfa1->states[i];
			if (s->type == e_final_state)
			{
				s->type = e_intermediate_state;
				t_transition *t = create_transition(EPSILON, end);
				add_transition(&s, t);
			}
			if (s->type == e_initial_state)
				s->type = e_intermediate_state;
			add_state(&nfa, s);
			i++;
		}
		i = 0;
		while (nfa2->states && i < nfa2->state_count)
		{
			t_state *s = nfa2->states[i];
			if (s->type == e_final_state)
			{
				s->type = e_intermediate_state;
				t_transition *t = create_transition(EPSILON, end);
				add_transition(&s, t);
			}
			if (s->type == e_initial_state)
				s->type = e_intermediate_state;
			add_state(&nfa, s);
			i++;
		}
		push_nfa(nfa);
		return;
	}
	else
	{
		t_nfa *nfa = create_nfa();
		nfa->start = create_state(e_initial_state);
		nfa->end = create_state(e_final_state);
		add_state(&nfa, nfa->start);
		add_state(&nfa, nfa->end);
		t_transition *transition = create_transition(c, nfa->end);
		add_transition(&nfa->start, transition);
		push_nfa(nfa);
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

// t_dfa *create_dfa_from_nfa(t_nfa *nfa)
// {
// }