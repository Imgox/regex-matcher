#include "mptlc.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
        throw_error(ERR_USAGE);

    char *regex = argv[1];
    char *word = argv[2];
    t_btree_node *tree;
    t_dfa *dfa;

    regex = insert_concat(regex);
    tree = regex_to_btree(regex);
    if (!tree)
        throw_error(ERR_EMPTY_TREE);
    btree_apply_postfix(tree, &ft_putchar);
    ft_putchar('\n');
    thompson(tree);
    show_stack();
    dfa = create_dfa_from_nfa(pop_nfa());
    print_dfa(dfa);
    int result = recognize(dfa->start, word);
    if (result)
        printf("\n%sMot accepté%s\n\n", GREEN, RESET);
    else
        printf("\n%sMot non accepté%s\n\n", RED, RESET);
    free_btree(tree);

    return 0;
}