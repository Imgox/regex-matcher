#include "mptlc.h"

t_btree_node *create_btree_node(char c)
{
	t_btree_node *node;

	node = (t_btree_node *)malloc(sizeof(t_btree_node));
	if (node == NULL)
		throw_error(ERR_ALLOC);
	node->c = c;
	node->left = NULL;
	node->right = NULL;
	return (node);
}

void btree_apply_postfix(t_btree_node *root, void (*applyf)(char))
{
	if (root == NULL)
		return;
	btree_apply_postfix(root->left, applyf);
	btree_apply_postfix(root->right, applyf);
	applyf(root->c);
}

void free_btree_node(t_btree_node *node)
{
	if (node == NULL)
		return;
	free_btree_node(node->left);
	free_btree_node(node->right);
	free(node);
}

void free_btree(t_btree_node *root)
{
	if (root == NULL)
		return;
	free_btree(root->left);
	free_btree(root->right);
	free(root);
}

static int calculate_precedence(char c)
{
	if (c == OR)
		return (1);
	if (c == CONCAT)
		return (2);
	if (c == STAR)
		return (3);
	return (0);
}

t_btree_node *handle_operator(t_btree_stack *stack, char operator)
{
	if (operator== OR)
	{
		if (stack->size < 2)
			throw_error(ERR_INV_REGEX);
		t_btree_node *right = pop_btree_stack(stack);
		t_btree_node *left = pop_btree_stack(stack);
		t_btree_node *node = create_btree_node(operator);
		node->left = left;
		node->right = right;
		return (node);
	}
	else if (operator== CONCAT)
	{
		if (stack->size < 2)
			throw_error(ERR_INV_REGEX);
		t_btree_node *right = pop_btree_stack(stack);
		t_btree_node *left = pop_btree_stack(stack);
		t_btree_node *node = create_btree_node(operator);
		node->left = left;
		node->right = right;
		return (node);
	}
	else if (operator== STAR)
	{
		if (stack->size < 1)
			throw_error(ERR_INV_REGEX);
		t_btree_node *node = create_btree_node(operator);
		node->left = pop_btree_stack(stack);
		return (node);
	}
	return NULL;
}

// Shunting-yard algorithm
void regex_to_btree_shunting_yard(t_btree_stack *btree_stack, t_operator_stack *operator_stack, char **regex)
{
	char c = **regex;

	if (is_op(c) || c == STAR)
	{
		if (operator_stack->size > 0)
		{
			while (calculate_precedence(c) <= calculate_precedence(top_operator_stack(*operator_stack)))
			{
				char ctop = pop_operator_stack(operator_stack);
				t_btree_node *res = handle_operator(btree_stack, ctop);
				push_btree_stack(btree_stack, res);
				if (operator_stack->size == 0)
					break;
			}
		}
		push_operator_stack(operator_stack, c);
	}
	else if (c == OPEN_PARENTHESIS)
		push_operator_stack(operator_stack, c);
	else if (c == CLOSE_PARENTHESIS)
	{
		while (top_operator_stack(*operator_stack) != OPEN_PARENTHESIS)
		{
			if (operator_stack->size == 0)
				throw_error(ERR_INV_REGEX);
			char ctop = pop_operator_stack(operator_stack);
			t_btree_node *res = handle_operator(btree_stack, ctop);
			push_btree_stack(btree_stack, res);
		}
		pop_operator_stack(operator_stack);
	}
	else
		push_btree_stack(btree_stack, create_btree_node(c));
	(*regex)++;
}

t_btree_node *regex_to_btree(char *regex)
{
	t_btree_stack btree_stack = create_btree_stack();
	t_operator_stack operator_stack = create_operator_stack();

	while (regex && regex[0])
		regex_to_btree_shunting_yard(&btree_stack, &operator_stack, &regex);

	while (operator_stack.size > 0)
	{
		char ctop = pop_operator_stack(&operator_stack);
		t_btree_node *res = handle_operator(&btree_stack, ctop);
		push_btree_stack(&btree_stack, res);
	}
	if (btree_stack.size != 1)
		throw_error(ERR_INV_REGEX);
	return (pop_btree_stack(&btree_stack));
}

t_btree_stack create_btree_stack()
{
	t_btree_stack stack;

	stack.content = NULL;
	stack.size = 0;

	return stack;
}

t_operator_stack create_operator_stack()
{
	t_operator_stack stack;

	stack.content = NULL;
	stack.size = 0;

	return stack;
}

void push_btree_stack(t_btree_stack *stack, t_btree_node *el)
{
	if (stack->size == 0)
	{
		stack->size++;
		stack->content = (t_btree_node **)malloc(sizeof(t_btree_node *));
		if (!stack->content)
			throw_error(ERR_ALLOC);
		stack->content[0] = el;
		return;
	}
	stack->size++;
	stack->content = (t_btree_node **)realloc(stack->content, sizeof(t_btree_node *) * stack->size);
	// if (!stack->content)
	// 	throw_error(ERR_ALLOC);
	stack->content[stack->size - 1] = el;
}

t_btree_node *pop_btree_stack(t_btree_stack *stack)
{
	if (stack->size == 0)
		return NULL;
	t_btree_node *el = stack->content[stack->size - 1];
	stack->size--;
	stack->content = (t_btree_node **)realloc(stack->content, sizeof(t_btree_node *) * stack->size);
	// if (stack->content == NULL)
	// 	throw_error(ERR_ALLOC);

	return el;
}

char top_operator_stack(t_operator_stack stack)
{
	if (stack.size == 0)
		return '\0';
	return stack.content[stack.size - 1];
}

void push_operator_stack(t_operator_stack *stack, char el)
{
	if (stack->size == 0)
	{
		stack->size++;
		stack->content = (char *)malloc(sizeof(char));
		if (!stack->content)
			throw_error(ERR_ALLOC);
		stack->content[0] = el;
		return;
	}
	stack->size++;
	stack->content = (char *)realloc(stack->content, sizeof(char) * stack->size);
	// if (!stack->content)
	// 	throw_error(ERR_ALLOC);
	stack->content[stack->size - 1] = el;
}

char pop_operator_stack(t_operator_stack *stack)
{
	if (stack->size == 0)
		return '\0';
	char el = stack->content[stack->size - 1];
	stack->size--;
	stack->content = (char *)realloc(stack->content, sizeof(char) * stack->size);
	// if (!stack->content)
	// 	throw_error(ERR_ALLOC);
	return el;
}