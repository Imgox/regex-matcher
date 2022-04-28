#ifndef DFA_H
#define DFA_H

typedef struct s_dfa
{
	t_state *start;
	t_state **states;
	t_state **ends;
	int state_count;
	int end_count;
} t_dfa;

void print_dfa(t_dfa *dfa);
t_dfa *create_dfa();
t_dfa *create_dfa_from_nfa(t_nfa *nfa);
void add_state_to_dfa(t_dfa **dfa, t_state *state);
void next_groups(t_dfa **dfa, t_state_group ***groups, int *group_count, t_state_group *start);

#endif // DFA_H