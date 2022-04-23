#include "mptlc.h"

static void test(int number, t_dfa *dfa, char *word)
{
    int result = recognize(dfa->start, word);
    if (result)
        printf("[%d]\t%sMot accepté%s\n", number + 1, GREEN, RESET);
    else
        printf("[%d]\t%sMot non accepté%s\n", number + 1, RED, RESET);
}

int main(int argc, char *argv[])
{
    char *regex = (char *)malloc(sizeof(char) * 20);
    char *word = (char *)malloc(sizeof(char) * 40);
    t_btree_node *tree;
    t_dfa *dfa;
    FILE *accept_file;
    FILE *reject_file;

    accept_file = fopen("./tests/accept.txt", "r");
    reject_file = fopen("./tests/reject.txt", "r");
    if (!accept_file || !reject_file)
        throw_error(ERR_TESTS_NOT_FOUND);

    printf("=========================\n");
    printf("   SHOUD BE ACCEPTED\n");
    printf("=========================\n");
    int i = 0;
    while (fscanf(accept_file, "%s %s", regex, word) != EOF)
    {
        regex = insert_concat(regex);
        tree = regex_to_btree(regex);
        if (!tree)
            throw_error(ERR_EMPTY_TREE);
        thompson(tree);
        dfa = create_dfa_from_nfa(pop_nfa());
        test(i, dfa, word);
        i++;
    }
    printf("=========================\n");
    printf("   SHOUD BE REJECTED\n");
    printf("=========================\n");
    i = 0;
    while (fscanf(reject_file, "%s %s", regex, word) != EOF)
    {
        regex = insert_concat(regex);
        tree = regex_to_btree(regex);
        if (!tree)
            throw_error(ERR_EMPTY_TREE);
        thompson(tree);
        dfa = create_dfa_from_nfa(pop_nfa());
        test(i, dfa, word);
        i++;
    }

    fclose(accept_file);
    fclose(reject_file);
    free(regex);
    free(word);
    free_btree(tree);

    return 0;
}
