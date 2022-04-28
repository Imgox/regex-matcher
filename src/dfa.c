#include "mptlc.h"

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
			// if ((*dfa)->ends == NULL)
			// 	throw_error(ERR_ALLOC);
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
	// if ((*dfa)->states == NULL)
	// 	throw_error(ERR_ALLOC);
	(*dfa)->states[(*dfa)->state_count] = state;
	(*dfa)->state_count++;
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