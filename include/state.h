#ifndef STATE_H
#define STATE_H

typedef enum e_state_type
{
	e_initial_state,
	e_final_state,
	e_intermediate_state,
} t_state_type;

typedef struct s_transition
{
	char c;
	struct s_state *next;
} t_transition;

typedef struct s_state
{
	t_state_type type;
	t_transition **transitions;
	int transition_count;
} t_state;

typedef struct s_state_group
{
	t_state_type type;
	t_state **states;
	t_state *state_eq;
	int state_count;
} t_state_group;

t_transition *create_transition(char c, t_state *next);
void add_transition(t_state **state, t_transition *transition);
void update_transition(t_transition **transition, char c, t_state *next);
t_state *create_state(t_state_type type);
void free_state(t_state *state);
void free_states(t_state **states);
t_state_group *create_group(t_state_type type);
t_state *is_group_in_groups(t_state_group **groups, int group_count, t_state_group *group);
int is_state_in_group(t_state_group *group, t_state *state);
void add_state_to_group(t_state_group **group, t_state *state);
void print_group(t_state_group *group, int index);
void print_groups(t_state_group **groups, int group_count);
int contains_only_epsilon_transitions(t_state *state);
void epsilon_close(t_state_group **group, t_state *state);

#endif // STATE_H