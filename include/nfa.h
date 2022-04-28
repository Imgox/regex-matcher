#ifndef NFA_H
#define NFA_H

typedef enum e_nfa_type
{
	e_nfa,
	e_dfa
} t_nfa_type;

typedef struct s_nfa
{
	t_state *start;
	t_state **states;
	t_state *end;
	int state_count;
} t_nfa;

void add_state_to_nfa(t_nfa **nfa, t_state *state);
t_nfa *create_nfa();
void free_nfa(t_nfa *nfa);
void push_nfa(t_nfa *nfa);
t_nfa *pop_nfa();
t_nfa *top_nfa();
void show_stack();
void thompson_elem(t_btree_node *node);
void thompson(t_btree_node *tree);

#endif // NFA_H