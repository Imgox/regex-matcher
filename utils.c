#include "mptlc.h"

t_nfa **g_nfa_stack;
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
	if ((*nfa)->states == NULL)
		throw_error(ERR_ALLOC);
	(*nfa)->states[(*nfa)->state_count] = state;
	(*nfa)->state_count++;
}

void add_state_to_dfa(t_dfa **dfa, t_state *state)
{
	if (state->type == e_final_state)
	{
		if ((*dfa)->ends == NULL)
		{
			(*dfa)->ends = (t_state **)malloc(sizeof(t_state *));
			if ((*dfa)->ends == NULL)
				throw_error(ERR_ALLOC);
			(*dfa)->ends[0] = state;
			(*dfa)->end_count = 1;
		}
		else
		{
			(*dfa)->ends = (t_state **)realloc((*dfa)->ends, sizeof(t_state *) * ((*dfa)->end_count + 1));
			if ((*dfa)->ends == NULL)
				throw_error(ERR_ALLOC);
			(*dfa)->ends[(*dfa)->end_count] = state;
			(*dfa)->end_count++;
		}
	}
	else if (state->type == e_initial_state)
		(*dfa)->start = state;
	if ((*dfa)->states == NULL)
	{
		(*dfa)->states = (t_state **)malloc(sizeof(t_state *));
		if ((*dfa)->states == NULL)
			throw_error(ERR_ALLOC);
		(*dfa)->states[0] = state;
		(*dfa)->state_count = 1;
		return;
	}
	(*dfa)->states = (t_state **)realloc((*dfa)->states, sizeof(t_state *) * ((*dfa)->state_count + 1));
	if ((*dfa)->states == NULL)
		throw_error(ERR_ALLOC);
	(*dfa)->states[(*dfa)->state_count] = state;
	(*dfa)->state_count++;
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

char *substring(char *str, int end)
{
	char *sub;
	int i;

	i = 0;
	sub = (char *)malloc(sizeof(char) * (end + 1));
	if (sub == NULL)
		throw_error(ERR_ALLOC);
	while (i < end)
	{
		sub[i] = str[i];
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

int find_closing(char *str)
{
	int i = 0;
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
	if (g_nfa_stack == NULL)
		throw_error(ERR_ALLOC);
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

void print_dfa(t_dfa *dfa)
{
	printf("==========================================\n");
	printf("                     DFA\n");
	printf("==========================================\n\n");
	if (dfa == NULL)
		return;
	int i = 0;
	printf("Nbr of states: %d\n", dfa->state_count);
	while (dfa->states[i] && i < dfa->state_count)
	{
		printf("\n-----------------------------\n\n");
		printf("\tState %s%p%s\n", COLORS[dfa->states[i]->type], dfa->states[i], RESET);
		printf("\tType: %s\n", TYPES[dfa->states[i]->type]);
		printf("\tTransitions:\n");
		int j = 0;
		if (dfa->states[i]->transition_count == 0 || dfa->states[i]->transitions == NULL)
			printf("\t\tNone\n");
		else
			while (dfa->states[i]->transitions[j] && j < dfa->states[i]->transition_count)
			{
				t_state *next = dfa->states[i]->transitions[j]->next;
				char c = dfa->states[i]->transitions[j]->c;
				printf("\t\t%c -> %s%p%s\n", c, COLORS[next->type], next, RESET);
				j++;
			}
		printf("%s", RESET);
		i++;
	}
	printf("==========================================\n");
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

t_dfa *create_dfa()
{
	t_dfa *dfa = (t_dfa *)malloc(sizeof(t_dfa));
	if (!dfa)
		throw_error(ERR_ALLOC);

	dfa->start = NULL;
	dfa->states = NULL;
	dfa->ends = NULL;
	dfa->state_count = 0;
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
		return;
	if (!(*group)->states)
	{
		(*group)->state_count = 1;
		(*group)->states = (t_state **)malloc(sizeof(t_state *));
		if (!(*group)->states)
			throw_error(ERR_ALLOC);
	}
	else
	{
		(*group)->state_count++;
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

t_state *is_group_in_groups(t_state_group **groups, int group_count, t_state_group *group)
{
	for (int i = 0; i < group_count; i++)
	{
		if (groups[i]->state_count != group->state_count)
			continue;
		if (groups_eq(groups[i], group))
			return groups[i]->state_eq;
	}
	return NULL;
}

void print_group(t_state_group *group, int index)
{
	printf("------------------------------\n");
	printf("       GROUP %d %s\n\n", index + 1, TYPES[group->type]);
	printf("\nNbr of states: %d\n\n", group->state_count);
	printf("%s", COLORS[group->type]);
	for (int i = 0; i < group->state_count; i++)
	{
		t_state *s = group->states[i];
		printf("\tState %p\n", s);
	}
	printf("%s", RESET);
}

void print_groups(t_state_group **group, int group_count)
{
	printf("======================================\n");
	printf("             %d GROUPS:\n", group_count);
	printf("======================================\n");
	for (int i = 0; i < group_count; i++)
	{
		print_group(group[i], i);
	}
	printf("======================================\n");
}

t_btree_stack create_btree_stack()
{
	t_btree_stack stack;

	stack.content = NULL;
	stack.size = 0;

	return stack;
}

t_operator_stack create_operator_stack()
{
	t_operator_stack stack;

	stack.content = NULL;
	stack.size = 0;

	return stack;
}

void push_btree_stack(t_btree_stack *stack, t_btree_node *el)
{
	if (stack->size == 0)
	{
		stack->size++;
		stack->content = (t_btree_node **)malloc(sizeof(t_btree_node *));
		if (!stack->content)
			throw_error(ERR_ALLOC);
		stack->content[0] = el;
		return;
	}
	stack->size++;
	stack->content = (t_btree_node **)realloc(stack->content, sizeof(t_btree_node *) * stack->size);
	if (!stack->content)
		throw_error(ERR_ALLOC);
	stack->content[stack->size - 1] = el;
}

t_btree_node *pop_btree_stack(t_btree_stack *stack)
{
	if (stack->size == 0)
		return NULL;
	t_btree_node *el = stack->content[stack->size - 1];
	stack->size--;
	stack->content = (t_btree_node **)realloc(stack->content, sizeof(t_btree_node *) * stack->size);
	if (!stack->content)
		throw_error(ERR_ALLOC);
	return el;
}

char top_operator_stack(t_operator_stack stack)
{
	if (stack.size == 0)
		return '\0';
	return stack.content[stack.size - 1];
}

void push_operator_stack(t_operator_stack *stack, char el)
{
	if (stack->size == 0)
	{
		stack->size++;
		stack->content = (char *)malloc(sizeof(char));
		if (!stack->content)
			throw_error(ERR_ALLOC);
		stack->content[0] = el;
		return;
	}
	stack->size++;
	stack->content = (char *)realloc(stack->content, sizeof(char) * stack->size);
	if (!stack->content)
		throw_error(ERR_ALLOC);
	stack->content[stack->size - 1] = el;
}

char pop_operator_stack(t_operator_stack *stack)
{
	if (stack->size == 0)
		return '\0';
	char el = stack->content[stack->size - 1];
	stack->size--;
	stack->content = (char *)realloc(stack->content, sizeof(char) * stack->size);
	if (!stack->content)
		throw_error(ERR_ALLOC);
	return el;
}

int is_op(char c)
{
	if (c == OR || c == CONCAT)
		return 1;
	return 0;
}

static int get_concat_count(const char *regex)
{
	int count = 0;
	int len = strlen(regex);

	for (int i = 0; i < len; i++)
	{
		if (regex[i + 1])
		{
			char c1 = regex[i];
			char c2 = regex[i + 1];
			if (is_op(c1) || is_op(c2))
				continue;
			if (c1 == OPEN_PARENTHESIS)
				continue;
			if (c2 == CLOSE_PARENTHESIS)
				continue;
			if (c1 == CLOSE_PARENTHESIS && c2 == STAR)
				continue;
			if (c1 == CLOSE_PARENTHESIS && c2 == CLOSE_PARENTHESIS)
				continue;
			if (c2 == STAR)
				continue;
			count++;
		}
	}
	return count;
}

char *insert_concat(const char *regex)
{
	int init_len = strlen(regex);
	int concat_count = get_concat_count(regex);
	char *ret = (char *)malloc(sizeof(char) * (init_len + concat_count + 1));
	int j = 0;

	for (int i = 0; i < init_len; i++)
	{
		if (regex[i + 1])
		{
			char c1 = regex[i];
			char c2 = regex[i + 1];
			if (is_op(c1) || is_op(c2) ||
				(c1 == OPEN_PARENTHESIS) ||
				(c2 == CLOSE_PARENTHESIS) ||
				(c1 == CLOSE_PARENTHESIS && c2 == STAR) ||
				(c1 == CLOSE_PARENTHESIS && c2 == CLOSE_PARENTHESIS) ||
				(c2 == STAR))
			{
				ret[j] = c1;
				j++;
			}
			else
			{
				ret[j] = c1;
				ret[j + 1] = CONCAT;
				j += 2;
			}
		}
		else
		{
			ret[j] = regex[i];
			j++;
		}
	}
	ret[j] = '\0';

	return ret;
}

void ft_putchar(char c)
{
	putchar(c);
}