#include "mptlc.h"

static int calculate_precedence(char c)
{
	if (c == OR)
		return (1);
	if (c == CONCAT)
		return (2);
	if (c == STAR)
		return (3);
	return (0);
}

t_btree_node *handle_operator(t_btree_stack *stack, char operator)
{
	if (operator== OR)
	{
		if (stack->size < 2)
			throw_error(ERR_INV_REGEX);
		t_btree_node *right = pop_btree_stack(stack);
		t_btree_node *left = pop_btree_stack(stack);
		t_btree_node *node = create_btree_node(operator);
		node->left = left;
		node->right = right;
		return (node);
	}
	else if (operator== CONCAT)
	{
		if (stack->size < 2)
			throw_error(ERR_INV_REGEX);
		t_btree_node *right = pop_btree_stack(stack);
		t_btree_node *left = pop_btree_stack(stack);
		t_btree_node *node = create_btree_node(operator);
		node->left = left;
		node->right = right;
		return (node);
	}
	else if (operator== STAR)
	{
		if (stack->size < 1)
			throw_error(ERR_INV_REGEX);
		t_btree_node *node = create_btree_node(operator);
		node->left = pop_btree_stack(stack);
		return (node);
	}
	return NULL;
}

// Shunting-yard algorithm
void regex_to_btree_shunting_yard(t_btree_stack *btree_stack, t_operator_stack *operator_stack, char **regex)
{
	char c = **regex;

	if (is_op(c) || c == STAR)
	{
		if (operator_stack->size > 0)
		{
			while (calculate_precedence(c) <= calculate_precedence(top_operator_stack(*operator_stack)))
			{
				char ctop = pop_operator_stack(operator_stack);
				t_btree_node *res = handle_operator(btree_stack, ctop);
				push_btree_stack(btree_stack, res);
				if (operator_stack->size == 0)
					break;
			}
		}
		push_operator_stack(operator_stack, c);
	}
	else if (c == OPEN_PARENTHESIS)
		push_operator_stack(operator_stack, c);
	else if (c == CLOSE_PARENTHESIS)
	{
		while (top_operator_stack(*operator_stack) != OPEN_PARENTHESIS)
		{
			if (operator_stack->size == 0)
				throw_error(ERR_INV_REGEX);
			char ctop = pop_operator_stack(operator_stack);
			t_btree_node *res = handle_operator(btree_stack, ctop);
			push_btree_stack(btree_stack, res);
		}
		pop_operator_stack(operator_stack);
	}
	else
		push_btree_stack(btree_stack, create_btree_node(c));
	(*regex)++;
}

t_btree_node *regex_to_btree(char *regex)
{
	t_btree_stack btree_stack = create_btree_stack();
	t_operator_stack operator_stack = create_operator_stack();

	while (regex && regex[0])
		regex_to_btree_shunting_yard(&btree_stack, &operator_stack, &regex);
	while (operator_stack.size > 0)
	{
		char ctop = pop_operator_stack(&operator_stack);
		t_btree_node *res = handle_operator(&btree_stack, ctop);
		push_btree_stack(&btree_stack, res);
	}
	if (btree_stack.size != 1)
		throw_error(ERR_INV_REGEX);
	return (pop_btree_stack(&btree_stack));
}

void thompson_elem(t_btree_node *node)
{
	char c = node->c;

	if (c == CONCAT)
	{
		t_nfa *nfa2 = pop_nfa();
		t_nfa *nfa1 = pop_nfa();
		t_nfa *nfa = create_nfa(e_nfa);
		int i = 0;
		while (i < nfa1->state_count)
		{
			t_state *s = nfa1->states[i];
			if (s->type != e_final_state)
				add_state_to_nfa(&nfa, s);
			int j = 0;
			while (j < s->transition_count)
			{
				t_transition *t = s->transitions[j];
				if (t->next->type == e_final_state)
					update_transition(&t, t->c, nfa2->start);
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
			add_state_to_nfa(&nfa, s);
			i++;
		}
		push_nfa(nfa);
		return;
	}
	else if (c == STAR)
	{
		t_nfa *nfa1 = pop_nfa();
		t_nfa *nfa = create_nfa(e_nfa);
		t_state *start = create_state(e_initial_state);
		t_state *end = create_state(e_final_state);
		t_transition *t = create_transition(EPSILON, nfa1->start);
		add_transition(&start, t);
		t = create_transition(EPSILON, end);
		add_transition(&start, t);
		add_state_to_nfa(&nfa, start);
		add_state_to_nfa(&nfa, end);
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
			add_state_to_nfa(&nfa, s);
			i++;
		}
		push_nfa(nfa);
		return;
	}
	else if (c == OR)
	{
		t_nfa *nfa2 = pop_nfa();
		t_nfa *nfa1 = pop_nfa();
		t_nfa *nfa = create_nfa(e_nfa);
		t_state *start = create_state(e_initial_state);
		t_state *end = create_state(e_final_state);
		t_transition *t = create_transition(EPSILON, nfa1->start);
		add_transition(&start, t);
		t = create_transition(EPSILON, nfa2->start);
		add_transition(&start, t);
		add_state_to_nfa(&nfa, start);
		add_state_to_nfa(&nfa, end);
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
			add_state_to_nfa(&nfa, s);
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
			add_state_to_nfa(&nfa, s);
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
		add_state_to_nfa(&nfa, nfa->start);
		add_state_to_nfa(&nfa, nfa->end);
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

void epsilon_close(t_state_group **group, t_state *state)
{
	if (is_state_in_group(*group, state))
		return;
	add_state_to_group(group, state);
	for (int i = 0; i < state->transition_count; i++)
	{
		t_transition *t = state->transitions[i];
		if (t->c != EPSILON)
			return;
		epsilon_close(group, t->next);
	}
}

void next_groups(t_dfa **dfa, t_state_group ***groups, int *group_count, t_state_group *start)
{
	if (!start)
		return;
	t_state *eq;

	t_state_group *group1 = create_group(e_intermediate_state);
	for (int i = 0; i < start->state_count; i++)
	{
		t_state *state = start->states[i];
		for (int j = 0; j < state->transition_count; j++)
		{
			t_transition *t = state->transitions[j];
			if (t->c == A)
				epsilon_close(&group1, t->next);
		}
	}

	t_state_group *group2 = create_group(e_intermediate_state);
	for (int i = 0; i < start->state_count; i++)
	{
		t_state *state = start->states[i];
		for (int j = 0; j < state->transition_count; j++)
		{
			t_transition *t = state->transitions[j];
			if (t->c == B)
				epsilon_close(&group2, t->next);
		}
	}
	if ((eq = is_group_in_groups(*groups, *group_count, group1)))
	{
		t_transition *t = create_transition(A, eq);
		add_transition(&start->state_eq, t);
	}
	else
	{
		if (group1->state_count > 0)
		{
			t_state *state = create_state(group1->type);
			t_transition *t = create_transition(A, state);
			add_transition(&start->state_eq, t);
			add_state_to_dfa(dfa, state);
			group1->state_eq = state;
			(*groups)[*group_count] = group1;
			(*group_count)++;
			next_groups(dfa, groups, group_count, group1);
		}
	}
	if ((eq = is_group_in_groups(*groups, *group_count, group2)))
	{
		t_transition *t = create_transition(B, eq);
		add_transition(&start->state_eq, t);
	}
	else
	{
		if (group2->state_count > 0)
		{
			t_state *state = create_state(group2->type);
			t_transition *t = create_transition(B, state);
			add_transition(&start->state_eq, t);
			add_state_to_dfa(dfa, state);
			group2->state_eq = state;
			(*groups)[*group_count] = group2;
			(*group_count)++;
			next_groups(dfa, groups, group_count, group2);
		}
	}
}

t_dfa *create_dfa_from_nfa(t_nfa *nfa)
{
	int group_count = 0;
	t_state_group **groups = (t_state_group **)malloc(sizeof(t_state_group *) * nfa->state_count);
	if (!groups)
		throw_error(ERR_ALLOC);
	t_dfa *dfa = create_dfa();

	groups[0] = create_group(e_initial_state);
	epsilon_close(&groups[0], nfa->start);
	group_count++;
	t_state *state = create_state(groups[0]->type);
	add_state_to_dfa(&dfa, state);
	dfa->start = state;
	groups[0]->state_eq = state;

	next_groups(&dfa, &groups, &group_count, groups[0]);
	// print_groups(groups, group_count);
	return dfa;
}

int recognize(t_state *current_state, char *word)
{
	if (!word || !word[0])
	{
		if (current_state->type == e_final_state)
			return 1;
		return 0;
	}
	char c = word[0];
	for (int i = 0; i < current_state->transition_count; i++)
	{
		t_transition *t = current_state->transitions[i];
		if (t->c == c)
			return recognize(t->next, word + 1);
	}
	return 0;
}