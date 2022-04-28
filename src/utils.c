#include "mptlc.h"

char *substring(char *str, int end)
{
	char *sub;
	int i;

	i = 0;
	sub = (char *)malloc(sizeof(char) * (end + 1));
	if (sub == NULL)
		throw_error(ERR_ALLOC);
	while (i < end)
	{
		sub[i] = str[i];
		i++;
	}
	sub[i] = '\0';
	return (sub);
}

void throw_error(char *str)
{
	fprintf(stderr, "%s\n", str);
	exit(1);
}

int find_closing(char *str)
{
	int i = 0;
	int count = 1;

	while (str[i])
	{
		if (str[i] == OPEN_PARENTHESIS)
			count++;
		if (str[i] == CLOSE_PARENTHESIS)
		{
			if (count == 1)
				return i;
			count--;
		}
		if (count == 0)
			return (i);
		i++;
	}
	return (-1);
}

int find_char(char *str, char c, int start)
{
	int i = start;
	while (str[i])
	{
		if (str[i] == c)
			return i;
		i++;
	}
	return -1;
}

int is_op(char c)
{
	if (c == OR || c == CONCAT)
		return 1;
	return 0;
}

static int get_concat_count(const char *regex)
{
	int count = 0;
	int len = strlen(regex);

	for (int i = 0; i < len; i++)
	{
		if (regex[i + 1])
		{
			char c1 = regex[i];
			char c2 = regex[i + 1];
			if (is_op(c1) || is_op(c2))
				continue;
			if (c1 == OPEN_PARENTHESIS)
				continue;
			if (c2 == CLOSE_PARENTHESIS)
				continue;
			if (c1 == CLOSE_PARENTHESIS && c2 == STAR)
				continue;
			if (c1 == CLOSE_PARENTHESIS && c2 == CLOSE_PARENTHESIS)
				continue;
			if (c2 == STAR)
				continue;
			count++;
		}
	}
	return count;
}

char *insert_concat(const char *regex)
{
	int init_len = strlen(regex);
	int concat_count = get_concat_count(regex);
	char *ret = (char *)malloc(sizeof(char) * (init_len + concat_count + 1));
	int j = 0;

	for (int i = 0; i < init_len; i++)
	{
		if (regex[i + 1])
		{
			char c1 = regex[i];
			char c2 = regex[i + 1];
			if (is_op(c1) || is_op(c2) ||
				(c1 == OPEN_PARENTHESIS) ||
				(c2 == CLOSE_PARENTHESIS) ||
				(c1 == CLOSE_PARENTHESIS && c2 == STAR) ||
				(c1 == CLOSE_PARENTHESIS && c2 == CLOSE_PARENTHESIS) ||
				(c2 == STAR))
			{
				ret[j] = c1;
				j++;
			}
			else
			{
				ret[j] = c1;
				ret[j + 1] = CONCAT;
				j += 2;
			}
		}
		else
		{
			ret[j] = regex[i];
			j++;
		}
	}
	ret[j] = '\0';

	return ret;
}

void ft_putchar(char c)
{
	putchar(c);
}