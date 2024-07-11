/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_lexer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvpee <mvpee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/13 09:38:20 by mvpee             #+#    #+#             */
/*   Updated: 2024/03/14 14:36:54 by mvpee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/minishell.h"

static void	set_lexer_quotes(char *str, int *i, char **buffer, char c)
{
	if (c == '\"')
	{
		*buffer = ft_strjoinchar_free(buffer, str[(*i)++]);
		while (str[(*i)] != '\"' && str[(*i)])
		{
			if (str[(*i)] == '\\' && str[(*i) + 1])
				*buffer = ft_strjoinchar_free(buffer, str[(*i)++]);
			*buffer = ft_strjoinchar_free(buffer, str[(*i)++]);
		}
		*buffer = ft_strjoinchar_free(buffer, str[(*i)]);
	}
	else
	{
		*buffer = ft_strjoinchar_free(buffer, str[(*i)++]);
		while (str[(*i)] != c && str[(*i)])
			*buffer = ft_strjoinchar_free(buffer, str[(*i)++]);
		*buffer = ft_strjoinchar_free(buffer, str[(*i)]);
	}
}

static void	set_lexer_redirection1(char *str, int *i, t_lexer *new_lexer, \
		t_token token)
{
	char	*file_name;

	file_name = NULL;
	while (str[(*i)] == ' ')
		(*i)++;
	while (str[(*i)] != ' ' && str[(*i)])
	{
		if (str[(*i)] == '\'')
		{
			file_name = ft_strjoinchar_free(&file_name, str[(*i)++]);
			while (str[(*i)] != '\'' && str[(*i)])
				file_name = ft_strjoinchar_free(&file_name, str[(*i)++]);
			file_name = ft_strjoinchar_free(&file_name, str[(*i)++]);
		}
		else if (str[(*i)] == '\"')
		{
			file_name = ft_strjoinchar_free(&file_name, str[(*i)++]);
			while (str[(*i)] != '\"' && str[(*i)])
				file_name = ft_strjoinchar_free(&file_name, str[(*i)++]);
			file_name = ft_strjoinchar_free(&file_name, str[(*i)++]);
		}
		else
			file_name = ft_strjoinchar_free(&file_name, str[(*i)++]);
	}
	append_node(&(new_lexer->head), &file_name, token);
}

static void	set_lexer_redirection(char *str, int *i, t_lexer *new_lexer, \
		t_token token)
{
	char	c;

	if (token == 1)
		c = '<';
	else
		c = '>';
	(*i)++;
	if (str[(*i)] == c)
	{
		(*i)++;
		set_lexer_redirection1(str, i, new_lexer, token + 1);
	}
	else
		set_lexer_redirection1(str, i, new_lexer, token);
	(*i)--;
}

static void	set_lexer_else(char *str, int *i, char **buffer)
{
	if (str[(*i)] == '\\' && str[(*i) + 1])
		*buffer = ft_strjoinchar_free(buffer, str[(*i)++]);
	*buffer = ft_strjoinchar_free(buffer, str[(*i)]);
}

t_lexer	set_lexer(char *str)
{
	t_lexer	new_lexer;
	int		i;
	char	*buffer;

	new_lexer.cmd = NULL;
	new_lexer.head = NULL;
	buffer = NULL;
	i = -1;
	while (str[++i])
	{
		if (str[i] == '\"')
			set_lexer_quotes(str, &i, &buffer, '\"');
		else if (str[i] == '\'')
			set_lexer_quotes(str, &i, &buffer, '\'');
		else if (str[i] == '<')
			set_lexer_redirection(str, &i, &new_lexer, INPUT);
		else if (str[i] == '>')
			set_lexer_redirection(str, &i, &new_lexer, OUTPUT);
		else
			set_lexer_else(str, &i, &buffer);
		if (!str[i])
			break ;
	}
	new_lexer.cmd = buffer;
	return (new_lexer);
}
