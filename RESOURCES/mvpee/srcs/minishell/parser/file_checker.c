/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file_checker.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvan-pee <mvan-pee@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/08 17:37:09 by mvpee             #+#    #+#             */
/*   Updated: 2024/03/18 11:57:04 by mvan-pee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/minishell.h"

static int	ft_open(char *file, t_token token)
{
	if (token == INPUT)
		return (open(file, O_RDONLY));
	else if (token == OUTPUT)
		return (open(file, O_CREAT | O_TRUNC | O_WRONLY, 0644));
	else if (token == APPEND)
		return (open(file, O_CREAT | O_APPEND | O_WRONLY, 0644));
	else if (token == HEREDOC)
		return (open(".heredoc", O_CREAT | O_TRUNC | O_WRONLY, 0644));
	return (-1);
}

static bool	heredoc_check(t_parser *parser, t_node *node, t_env *env, \
		t_data *data)
{
	if (node->token == HEREDOC)
	{
		ft_heredoc(parser->input, &node->name, env, data);
		close(parser->input);
		parser->input = open(".heredoc", O_RDONLY, 0644);
		if (parser->input == -1)
			return (ft_printf_fd(2, RED "CHMOD 777 .heredoc\n" RESET), false);
	}
	return (true);
}

static bool	input_check(t_parser *parser, t_node *node, t_env *env, \
		t_data *data)
{
	char	**temp;

	if (parser->input != -1)
		close(parser->input);
	temp = ft_expand(node->name, env, *data);
	if (node->token == INPUT && !temp && !only_space(node->name))
		return (ft_printf_fd(2, "%s: ambiguous redirect\n", node->name), \
		parser->flag = true, ft_free_matrix(1, &temp), true);
	ft_free_matrix(1, &temp);
	parser->input = ft_open(node->name, node->token);
	if (parser->input == -1)
	{
		if (node->token == INPUT)
		{
			perror(node->name);
			parser->flag = true;
		}
		else
			return (ft_printf_fd(2, RED "CHMOD 777 .heredoc\n" RESET), false);
	}
	if (!heredoc_check(parser, node, env, data))
		return (false);
	if (data->env_var == COMMAND_INTERRUPTED)
		return (false);
	return (true);
}

static bool	output_check(t_parser *parser, t_node *node, t_env *env, \
		t_data *data)
{
	char	**temp;

	if (parser->output != -1)
		close(parser->output);
	temp = ft_expand(node->name, env, *data);
	if (!only_space(node->name) && !temp)
		return (ft_printf_fd(2, "%s: ambiguous redirect\n", node->name), \
		parser->flag = true, ft_free_matrix(1, &temp), true);
	ft_free_matrix(1, &temp);
	parser->output = ft_open(node->name, node->token);
	if (parser->output == -1)
	{
		perror(node->name);
		parser->flag = true;
	}
	return (false);
}

bool	file_checker(t_parser *parser, t_lexer lexer, t_env *env, t_data *data)
{
	t_node	*node;

	node = NULL;
	node = lexer.head;
	while (node)
	{
		if (node->token == INPUT || node->token == HEREDOC)
		{
			if (!input_check(parser, node, env, data))
				return (false);
			if (parser->flag)
				return (true);
		}
		if (node->token == OUTPUT || node->token == APPEND)
		{
			if (output_check(parser, node, env, data))
				return (true);
			if (parser->flag)
				return (true);
		}
		node = node->next;
	}
	return (true);
}
