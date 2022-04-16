#include "mptlc.h"

t_nfa **g_nfa_stack;
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

t_state *create_state(t_type type)
{
	t_state *state = (t_state *)malloc(sizeof(t_state));
	state->type = type;
	state->transition_count = 0;
	state->transitions = NULL;
	return (state);
}

void add_state(t_nfa **nfa, t_state *state)
{
	if ((*nfa)->intermediates == NULL)
	{
		(*nfa)->intermediates = (t_state **)malloc(sizeof(t_state *));
		if ((*nfa)->intermediates == NULL)
			throw_error(ERR_ALLOC);
		(*nfa)->intermediates[0] = state;
		(*nfa)->state_count = 1;
		return;
	}
	(*nfa)->intermediates = (t_state **)realloc((*nfa)->intermediates, sizeof(t_state *) * ((*nfa)->state_count + 1));
	if ((*nfa)->intermediates == NULL)
		throw_error(ERR_ALLOC);
	(*nfa)->intermediates[(*nfa)->state_count] = state;
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

t_nfa *create_nfa()
{
	t_nfa *nfa = (t_nfa *)malloc(sizeof(t_nfa));
	nfa->state_count = 0;
	nfa->intermediates = NULL;
	return (nfa);
}

void free_nfa(t_nfa *nfa)
{
	free_state(nfa->start);
	free_state(nfa->end);
	free_states(nfa->intermediates);
	free(nfa);
}

void push_nfa(t_nfa *nfa)
{
	g_nfa_stack = (t_nfa **)realloc(g_nfa_stack, (g_nfa_count + 1) * sizeof(t_nfa *));
	if (g_nfa_stack == NULL)
		throw_error(ERR_ALLOC);
	g_nfa_stack[g_nfa_count] = nfa;
	g_nfa_count++;
}

t_nfa *pop_nfa()
{
	g_nfa_count--;
	t_nfa *nfa = g_nfa_stack[g_nfa_count];
	// free_state(nfa->start);
	// free_state(nfa->end);
	// free_states(nfa->intermediates);
	// free(nfa);
	g_nfa_stack = realloc(g_nfa_stack, g_nfa_count * sizeof(t_nfa *));
	return nfa;
}

t_nfa *top_nfa()
{
	return (g_nfa_stack[g_nfa_count - 1]);
}

void print_nfa(t_nfa *nfa)
{
	char *types[] = {
		"Initial",
		"Final",
		"Intermediate",
	};
	if (nfa == NULL)
		return;
	int i = 0;
	printf("Nbr of states: %d\n", nfa->state_count);
	while (nfa->intermediates[i] && i < nfa->state_count)
	{
		printf("-------------\n");
		printf("State %p\n", nfa->intermediates[i]);
		printf("Type: %s\n", types[nfa->intermediates[i]->type]);
		printf("Transitions:\n");
		int j = 0;
		if (nfa->intermediates[i]->transition_count == 0 || nfa->intermediates[i]->transitions == NULL)
			printf("\tNone\n");
		else
			while (nfa->intermediates[i]->transitions[j] && j < nfa->intermediates[i]->transition_count)
			{
				printf("\t%c -> %p\n", nfa->intermediates[i]->transitions[j]->c, nfa->intermediates[i]->transitions[j]->next);
				j++;
			}
		i++;
	}
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