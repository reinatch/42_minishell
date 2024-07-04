/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_expand.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvpee <mvpee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/03 12:12:13 by mvpee             #+#    #+#             */
/*   Updated: 2024/03/17 20:17:59 by mvpee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../../includes/minishell.h"

static char	*ft_expand_double_quotes(char *line, t_env *head, t_data data,
		int *i)
{
	char	*buffer;

	buffer = NULL;
	while (line[++(*i)] != '\"' && line[*i])
	{
		if (line[*i] == '\\' && line[*i + 1])
			buffer = ft_strjoinchar_free(&buffer, line[++(*i)]);
		else if (line[*i] == '$' && (ft_isalpha(line[*i + 1]) \
			|| line[*i + 1] == '?'))
			buffer = ft_strjoin_ff(&buffer, exp_env(line, head, data, i));
		else
			buffer = ft_strjoinchar_free(&buffer, line[*i]);
	}
	return (buffer);
}

char	*ft_exp2(char *line, t_env *head, t_data data, int *i)
{
	char	*buffer;

	buffer = NULL;
	if (line[*i] == '\\')
	{
		if (line[++(*i)])
			buffer = ft_strjoinchar_free(&buffer, line[(*i)]);
	}
	else if (line[*i] == '\"')
		buffer = ft_strjoin_ff(&buffer, ft_expand_double_quotes(line, head, \
				data, i));
	else if (line[*i] == '\'')
		while (line[++(*i)] != '\'')
			buffer = ft_strjoinchar_free(&buffer, line[*i]);
	else if (ft_isprint(line[*i]) && line[*i] != '\'' && line[*i] != '\"'
		&& line[*i] != ' ' && line[*i] != '\\')
		buffer = ft_strjoinchar_free(&buffer, line[*i]);
	return (buffer);
}

static void	split_env_var(char **buffer, char ***split)
{
	char	**split_temp;
	int		j;

	if (!(*buffer) || !buffer)
		return ;
	split_temp = ft_split(*buffer, " ");
	if (!split_temp)
	{
		ft_free(1, buffer);
		ft_free_matrix(1, split);
		return ;
	}
	j = -1;
	while (split_temp[++j])
		*split = ft_splitjoin(*split, split_temp[j]);
	ft_free_matrix(1, &split_temp);
	ft_free(1, buffer);
}

static bool	ft_expand3(char c, char **buffer, char ***split)
{
	if (c == ' ' && c)
	{
		if (*buffer)
			*split = ft_splitjoin(*split, *buffer);
		ft_free(1, buffer);
	}
	if (!c)
		return (true);
	return (false);
}

char	**ft_expand(char *line, t_env *head, t_data data)
{
	char	**split;
	char	*buffer;
	int		i;

	ft_null(2, &buffer, &split);
	i = -1;
	while (line[++i])
	{
		ft_expand_home(line, &i, &buffer);
		if (line[i] != ' ')
		{
			if (line[i] == '$')
			{
				buffer = ft_strjoin_ff(&buffer, exp_env(line, head, data, &i));
				split_env_var(&buffer, &split);
			}
			else
				buffer = ft_strjoin_ff(&buffer, ft_exp2(line, head, data, &i));
		}
		if (ft_expand3(line[i], &buffer, &split))
			break ;
	}
	if (buffer)
		split = ft_splitjoin(split, buffer);
	return (ft_free(1, &buffer), split);
}
