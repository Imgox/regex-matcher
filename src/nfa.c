#include "mptlc.h"

t_nfa **g_nfa_stack;
int g_nfa_count = 0;

t_nfa *create_nfa()
{
	t_nfa *nfa = (t_nfa *)malloc(sizeof(t_nfa));
	nfa->state_count = 0;
	nfa->states = NULL;
	nfa->start = NULL;
	nfa->end = NULL;
	return (nfa);
}

void free_nfa(t_nfa *nfa)
{
	free_state(nfa->start);
	free_state(nfa->end);
	free_states(nfa->states);
	free(nfa);
}

void push_nfa(t_nfa *nfa)
{
	g_nfa_stack = (t_nfa **)realloc(g_nfa_stack, (g_nfa_count + 1) * sizeof(t_nfa *));
	// if (g_nfa_stack == NULL)
	// 	throw_error(ERR_ALLOC);
	g_nfa_stack[g_nfa_count] = nfa;
	g_nfa_count++;
}

t_nfa *pop_nfa()
{
	g_nfa_count--;
	t_nfa *nfa = g_nfa_stack[g_nfa_count];
	g_nfa_stack = realloc(g_nfa_stack, g_nfa_count * sizeof(t_nfa *));
	return nfa;
}

t_nfa *top_nfa()
{
	return (g_nfa_stack[g_nfa_count - 1]);
}

void add_state_to_nfa(t_nfa **nfa, t_state *state)
{
	if (state->type == e_initial_state && !(*nfa)->start)
		(*nfa)->start = state;
	if (state->type == e_final_state && !(*nfa)->end)
		(*nfa)->end = state;
	if ((*nfa)->states == NULL)
	{
		(*nfa)->states = (t_state **)malloc(sizeof(t_state *));
		if ((*nfa)->states == NULL)
			throw_error(ERR_ALLOC);
		(*nfa)->states[0] = state;
		(*nfa)->state_count = 1;
		return;
	}
	(*nfa)->states = (t_state **)realloc((*nfa)->states, sizeof(t_state *) * ((*nfa)->state_count + 1));
	// if ((*nfa)->states == NULL)
	// 	throw_error(ERR_ALLOC);
	(*nfa)->states[(*nfa)->state_count] = state;
	(*nfa)->state_count++;
}

void print_nfa(t_nfa *nfa)
{
	if (nfa == NULL)
		return;
	int i = 0;
	printf("Nbr of states: %d\n", nfa->state_count);
	while (nfa->states[i] && i < nfa->state_count)
	{
		printf("\n-----------------------------\n\n");
		printf("\tState %s%p%s\n", COLORS[nfa->states[i]->type], nfa->states[i], RESET);
		printf("\tType: %s\n", TYPES[nfa->states[i]->type]);
		printf("\tTransitions:\n");
		int j = 0;
		if (nfa->states[i]->transition_count == 0 || nfa->states[i]->transitions == NULL)
			printf("\t\tNone\n");
		else
			while (nfa->states[i]->transitions[j] && j < nfa->states[i]->transition_count)
			{
				t_state *next = nfa->states[i]->transitions[j]->next;
				char c = nfa->states[i]->transitions[j]->c;
				printf("\t\t%c -> %s%p%s\n", c, COLORS[next->type], next, RESET);
				j++;
			}
		printf("%s", RESET);
		i++;
	}
}

void show_stack()
{
	for (int i = g_nfa_count - 1; i >= 0; i--)
	{
		printf("======================================\n");
		printf("   NFA No %d\n", g_nfa_count - 1 - i);
		printf("======================================\n\n");
		print_nfa(g_nfa_stack[i]);
		printf("\n======================================\n");
	}
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