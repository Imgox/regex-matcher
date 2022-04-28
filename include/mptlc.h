#ifndef MPTLC_H
#define MPTLC_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "constants.h"
#include "btree.h"
#include "state.h"
#include "nfa.h"
#include "dfa.h"
#include "utils.h"

int recognize(t_state *current_state, char *word);

#endif // UTILS_H