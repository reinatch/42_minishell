/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvan-pee <mvan-pee@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 14:01:16 by mvpee             #+#    #+#             */
/*   Updated: 2024/03/15 15:14:58 by mvan-pee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/minishell.h"

bool	only_space(char *line)
{
	int	i;

	i = -1;
	while (line[++i])
		if (line[i] != ' ')
			return (false);
	return (true);
}

static bool	is_only_space(char *line)
{
	int	i;

	if (line[0] == '\'')
	{
		i = 0;
		while (line[++i] != '\'')
		{
			if ((line[i]) != ' ')
				return (false);
		}
	}
	else if (line[0] == '\"')
	{
		i = 0;
		while (line[++i] != '\"')
		{
			if ((line[i]) != ' ')
				return (false);
		}
	}
	else
		return (false);
	return (true);
}

static char	**ft_expand_space(char *str)
{
	int		len;
	char	*result;
	char	**split;

	split = NULL;
	result = NULL;
	len = ft_strlen(str) - 2;
	if (!len)
		return (ft_splitjoin(split, ""));
	while (len--)
		result = ft_strjoinchar_free(&result, ' ');
	split = ft_splitjoin(split, result);
	ft_free(1, &result);
	return (split);
}

static t_parser	parser_data(t_lexer lexer, t_data *data, t_env *env)
{
	t_parser	parser;

	parser = init_parser();
	if (!file_checker(&parser, lexer, env, data))
		return (parser);
	if (!lexer.cmd)
		return (parser.cmd = NULL, parser);
	if (is_only_space(lexer.cmd))
	{
		parser.cmd = ft_expand_space(lexer.cmd);
		parser.path = NULL;
		return (parser);
	}
	else
		parser.cmd = ft_expand(lexer.cmd, env, *data);
	if (parser.cmd)
	{
		parser.isbuiltins = isbuiltins(parser.cmd[0]);
		parser.ft_isspecial = ft_isspecial(parser);
		parser.path = path_checker(ft_split((const char *) \
			get_value(find_key(env, "PATH")), ":"), parser);
	}
	else
		data->env_var = SUCCESSFUL_COMMAND;
	return (parser);
}

t_parser	*ft_parser(t_lexer *lexer, t_data *data, t_env *env)
{
	t_parser	*parser;
	int			i;

	if (g_flag == SIGINT)
	{
		g_flag = 0;
		data->env_var = UNSUCCESSFUL_COMMAND;
	}
	if (!lexer)
		return (data->env_var = NUMERICAL_ARGUMENT_NESCESSARY, NULL);
	parser = malloc(sizeof(t_parser) * data->nbr_cmd);
	if (!parser)
		return (free_lexer(lexer, data->nbr_cmd), NULL);
	i = -1;
	while (++i < data->nbr_cmd)
	{
		parser[i] = parser_data(lexer[i], data, env);
		if (data->flag)
			return (free_lexer(lexer, data->nbr_cmd), ft_free(1, &parser), \
				NULL);
		if (!parser[i].cmd)
			return (free_lexer(lexer, data->nbr_cmd), free_parser(parser, \
					*data), NULL);
	}
	return (free_lexer(lexer, data->nbr_cmd), parser);
}
