/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_check.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvpee <mvpee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/05 10:52:14 by mvpee             #+#    #+#             */
/*   Updated: 2024/03/16 17:43:33 by mvpee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../../includes/minishell.h"

static int	syntax_check_in(char *line, int *i)
{
	if (line[(*i)] == '<')
	{
		if (line[(*i) + 1] == '>')
			return (ft_printf("syntax error near unexpected token '<'\n"));
		if (line[(*i) + 1] == '<')
		{
			(*i) = (*i) + 2;
			while (line[(*i)] == ' ')
				(*i)++;
			if (ft_ischarin(line[(*i)], "<>|") || !line[(*i)])
				return (ft_printf("syntax error near unexpected token '<'\n"));
		}
		else
		{
			(*i)++;
			while (line[(*i)] == ' ')
				(*i)++;
			if (ft_ischarin(line[(*i)], "<>|") || !line[(*i)])
				return (ft_printf("syntax error near unexpected token '<'\n"));
		}
		(*i)--;
	}
	return (0);
}

static int	syntax_check_out(char *line, int *i)
{
	if (line[(*i)] == '>')
	{
		if (line[(*i) + 1] == '<')
			return (ft_printf("syntax error near unexpected token '>'\n"));
		if (line[(*i) + 1] == '>')
		{
			(*i) = (*i) + 2;
			while (line[(*i)] == ' ')
				(*i)++;
			if (ft_ischarin(line[(*i)], "<>|") || !line[(*i)])
				return (ft_printf("syntax error near unexpected token '>'\n"));
		}
		else
		{
			(*i)++;
			while (line[(*i)] == ' ')
				(*i)++;
			if (ft_ischarin(line[(*i)], "<>|") || !line[(*i)])
				return (ft_printf("syntax error near unexpected token '>'\n"));
		}
		i--;
	}
	return (0);
}

static int	syntax_check3(char *line, int *i)
{
	if (line[(*i)] == ';')
		return (ft_printf("syntax error near unexpected token `;'\n"));
	else if (line[(*i)] == '|')
	{
		if (check_before(line, (*i)))
			return (ft_printf("syntax error near unexpected token `|'\n"));
		if (check_after(line, (*i)))
			return (ft_printf("syntax error near unexpected token `|'\n"));
	}
	return (0);
}

static int	syntax_check2(char *line, int *i)
{
	if (line[(*i)] == '\"')
	{
		(*i)++;
		while (line[(*i)] && line[(*i)] != '\"')
		{
			if (line[(*i)] == '\\' && line[(*i) + 1])
				(*i)++;
			(*i)++;
		}
		if (!line[(*i)])
			return (ft_printf("syntax error unclosed \" token\n"));
	}
	else if (line[(*i)] == '\'')
	{
		(*i)++;
		while (line[(*i)] && line[(*i)] != '\'')
			(*i)++;
		if (!line[(*i)])
			return (ft_printf("syntax error unclosed \' token\n"));
	}
	if (syntax_check3(line, i))
		return (1);
	return (0);
}

bool	syntax_check(char *str)
{
	int		i;
	char	*line;

	line = ft_strtrim(str, " ");
	if (!line)
		return (true);
	i = -1;
	while (line[++i])
	{
		if (syntax_check_in(line, &i))
			return (ft_free(1, &line), true);
		if (syntax_check_out(line, &i))
			return (ft_free(1, &line), true);
		if (line[i] == '\\')
			i++;
		else if (syntax_check2(line, &i))
			return (ft_free(1, &line), true);
		if (!line[i])
			break ;
	}
	return (ft_free(1, &line), false);
}
