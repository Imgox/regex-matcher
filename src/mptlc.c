#include "mptlc.h"

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