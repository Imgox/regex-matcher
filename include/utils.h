#ifndef UTILS_H
#define UTILS_H

char *substring(char *str, int start);
void throw_error(char *str);
int find_closing(char *str);
int find_char(char *str, char c, int start);
char *insert_concat(const char *regex);
char *insert_paranthesis(const char *regex);
void ft_putchar(char c);

#endif