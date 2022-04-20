#include "mptlc.h"

t_automaton **g_nfa_stack;
int g_nfa_count = 0;

t_btree_node *create_btree_node(char c)
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

t_transition *create_transition(char c, t_state *next)
{
	t_transition *transition;

	transition = (t_transition *)malloc(sizeof(t_transition));
	if (transition == NULL)
		throw_error(ERR_ALLOC);
	transition->c = c;
	transition->next = next;
	return (transition);
}

void add_transition(t_state **state, t_transition *transition)
{
	if ((*state)->transitions == NULL)
	{
		(*state)->transitions = (t_transition **)malloc(sizeof(t_transition *));
		if ((*state)->transitions == NULL)
			throw_error(ERR_ALLOC);
		(*state)->transitions[0] = transition;
		(*state)->transition_count = 1;
		return;
	}
	(*state)->transitions = (t_transition **)realloc((*state)->transitions, sizeof(t_transition *) * ((*state)->transition_count + 1));
	if ((*state)->transitions == NULL)
		throw_error(ERR_ALLOC);
	(*state)->transitions[(*state)->transition_count] = transition;
	(*state)->transition_count++;
}

void update_transition(t_transition **transition, char c, t_state *next)
{
	(*transition)->c = c;
	(*transition)->next = next;
}

t_state *create_state(t_state_type type)
{
	t_state *state = (t_state *)malloc(sizeof(t_state));
	state->type = type;
	state->transition_count = 0;
	state->transitions = NULL;
	return (state);
}

void add_state(t_automaton **nfa, t_state *state)
{
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
	if ((*nfa)->states == NULL)
		throw_error(ERR_ALLOC);
	(*nfa)->states[(*nfa)->state_count] = state;
	(*nfa)->state_count++;
}

void free_state(t_state *state)
{
	int i;

	for (i = 0; i < state->transition_count; i++)
		free(state->transitions[i]);
	free(state->transitions);
	free(state);
}

void free_states(t_state **states)
{
	int i;

	i = 0;
	while (states[i])
	{
		free_state(states[i]);
		i++;
	}
	free(states);
}

void btree_apply_postfix(t_btree_node *root, void (*applyf)(char))
{
	if (root == NULL)
		return;
	btree_apply_postfix(root->left, applyf);
	btree_apply_postfix(root->right, applyf);
	applyf(root->c);
}

void free_btree_node(t_btree_node *node)
{
	if (node == NULL)
		return;
	free_btree_node(node->left);
	free_btree_node(node->right);
	free(node);
}

void free_btree(t_btree_node *root)
{
	if (root == NULL)
		return;
	free_btree(root->left);
	free_btree(root->right);
	free(root);
}

t_btree_node *copy_btree(t_btree_node *tree)
{
	t_btree_node *copy;

	if (tree == NULL)
		return NULL;

	copy = create_btree_node(tree->c);
	copy->left = copy_btree(tree->left);
	copy->right = copy_btree(tree->right);

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
		{
			if (count == 1)
				return i;
			count--;
		}
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

int find_next_or(char *str, int start)
{
	// ignore or between parentheses
	int i = start;
	while (str[i])
	{
		if (str[i] == OPEN_PARENTHESIS)
		{
			i = find_closing(str, i);
			if (i == -1)
				return -1;
		}
		if (str[i] == OR)
			return i;
		i++;
	}
	return -1;
}

t_automaton *create_automaton(t_automaton_type type)
{
	t_automaton *automaton = (t_automaton *)malloc(sizeof(t_automaton));
	automaton->type = type;
	automaton->state_count = 0;
	automaton->states = NULL;
	automaton->start = NULL;
	automaton->end = NULL;
	return (automaton);
}

void free_nfa(t_automaton *nfa)
{
	free_state(nfa->start);
	free_state(nfa->end);
	free_states(nfa->states);
	free(nfa);
}

void push_nfa(t_automaton *nfa)
{
	g_nfa_stack = (t_automaton **)realloc(g_nfa_stack, (g_nfa_count + 1) * sizeof(t_automaton *));
	if (g_nfa_stack == NULL)
		throw_error(ERR_ALLOC);
	g_nfa_stack[g_nfa_count] = nfa;
	g_nfa_count++;
}

t_automaton *pop_nfa()
{
	g_nfa_count--;
	t_automaton *nfa = g_nfa_stack[g_nfa_count];
	g_nfa_stack = realloc(g_nfa_stack, g_nfa_count * sizeof(t_automaton *));
	return nfa;
}

t_automaton *top_nfa()
{
	return (g_nfa_stack[g_nfa_count - 1]);
}

void print_automaton(t_automaton *nfa)
{
	if (nfa == NULL)
		return;
	int i = 0;
	printf("Nbr of states: %d\n", nfa->state_count);
	while (nfa->states[i] && i < nfa->state_count)
	{
		printf("\n-----------------------------\n\n");
		printf("%s", COLORS[nfa->states[i]->type]);
		printf("\tState %p\n", nfa->states[i]);
		printf("\tType: %s\n", TYPES[nfa->states[i]->type]);
		printf("\tTransitions:\n");
		int j = 0;
		if (nfa->states[i]->transition_count == 0 || nfa->states[i]->transitions == NULL)
			printf("\t\tNone\n");
		else
			while (nfa->states[i]->transitions[j] && j < nfa->states[i]->transition_count)
			{
				printf("\t\t%c -> %p\n", nfa->states[i]->transitions[j]->c, nfa->states[i]->transitions[j]->next);
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
		print_automaton(g_nfa_stack[i]);
		printf("\n======================================\n");
	}
}

t_dfa *create_dfa()
{
	t_dfa *dfa = (t_dfa *)malloc(sizeof(t_dfa));
	if (!dfa)
		throw_error(ERR_ALLOC);
	
	dfa->start = NULL;
	dfa->groups = NULL;
	dfa->end = NULL;
	dfa->group_count = 0;
	dfa->end_count = 0;

	return dfa;
}

t_state_group *create_group(t_state_type type)
{
	t_state_group *group = (t_state_group *)malloc(sizeof(t_state_group));
	if (!group)
		throw_error(ERR_ALLOC);
	
	group->states = NULL;
	group->state_count = 0;
	group->type = type;

	return group;
}

int is_state_in_group(t_state_group *group, t_state *state)
{
	if (!group)
		return 0;
	for (int i = 0; i < group->state_count; i++)
	{
		t_state *s = group->states[i];
		if (state == s)
			return 1;
	}
	return 0;
}

void add_state_to_group(t_state_group **group, t_state *state)
{
	if (!(*group))
		return ;
	(*group)->state_count++;
	if (!(*group)->states) {
		(*group)->states = (t_state **)malloc(sizeof(t_state *));
		if (!(*group)->states)
			throw_error(ERR_ALLOC);
	} else {
		(*group)->states = (t_state **)realloc((*group)->states, sizeof(t_state *) * (*group)->state_count);
		if (!(*group)->states)
			throw_error(ERR_ALLOC);
	}
	if (state->type == e_final_state)
		(*group)->type = e_final_state;
	(*group)->states[(*group)->state_count - 1] = state;
}

int groups_eq(t_state_group *group1, t_state_group *group2)
{
	int flag;

	for (int i = 0; i < group1->state_count; i++)
	{
		flag = 0;
		t_state *s = group1->states[i];
		if (!is_state_in_group(group2, s))
			return 0;
	}
	return 1;
}

int is_group_in_groups(t_state_group **groups, int group_count, t_state_group *group)
{
	for (int i = 0; i < group_count; i++)
	{
		if (groups[i]->state_count != group->state_count)
			return 0;
		if (groups_eq(groups[i], group))
			return 1;
	}
	return 0;
}

void add_group(t_dfa **dfa, t_state_group *group)
{
	if (!(*dfa))
		return;
	(*dfa)->group_count++;
	if (!(*dfa)->groups) {
		(*dfa)->groups = (t_state_group **)malloc(sizeof(t_state_group *));
		if (!(*dfa)->groups)
			throw_error(ERR_ALLOC);
	} else {
		(*dfa)->groups = (t_state_group **)realloc((*dfa)->groups, sizeof(t_state_group *) * (*dfa)->group_count);
		if (!(*dfa)->groups)
			throw_error(ERR_ALLOC);
	}
	(*dfa)->groups[(*dfa)->group_count - 1] = group;
	if (group->type = e_final_state)
		add_final_group(dfa, group);
}

void add_final_group(t_dfa **dfa, t_state_group *group)
{
	if (!(*dfa))
		return;
	(*dfa)->end_count++;
	if (!(*dfa)->end) {
		(*dfa)->end = (t_state_group **)malloc(sizeof(t_state_group *));
		if (!(*dfa)->end)
			throw_error(ERR_ALLOC);
	} else {
		(*dfa)->end = (t_state_group **)realloc((*dfa)->end, sizeof(t_state_group *) * (*dfa)->end_count);
		if (!(*dfa)->end)
			throw_error(ERR_ALLOC);
	}
	(*dfa)->end[(*dfa)->end_count - 1] = group;
}

void print_group(t_state_group *group)
{
	printf("============================\n");
	printf("       GROUP %s\n", TYPES[group->type]);
	printf("============================\n");
	printf("\nNbr of states: %d\n\n", group->state_count);
	printf("%s", COLORS[group->type]);
	for (int i = 0; i < group->state_count; i++)
	{
		t_state *s = group->states[i];
		printf("\tState %p\n", s);
	}
	printf("%s", RESET);	
}

void ft_putchar(char c)
{
	write(1, &c, 1);
}