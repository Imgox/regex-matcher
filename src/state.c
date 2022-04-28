#include "mptlc.h"

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
	// if ((*state)->transitions == NULL)
	// 	throw_error(ERR_ALLOC);
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
		// if (!(*group)->states)
		// 	throw_error(ERR_ALLOC);
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