/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exp_env.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvpee <mvpee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/08 15:12:55 by mvpee             #+#    #+#             */
/*   Updated: 2024/03/13 16:00:48 by mvpee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../../includes/minishell.h"

static void	exp_env2(char *line, t_env *head, int *i, char **buffer)
{
	char	*buffer2;
	char	*value;
	int		p;

	buffer2 = NULL;
	value = NULL;
	while (ft_isalnum(line[++(*i)]) || line[*i] == '_')
		buffer2 = ft_strjoinchar_free(&buffer2, line[*i]);
	if (find_key(head, buffer2))
	{
		value = ft_strdup(find_key(head, buffer2)->value);
		if (!value)
		{
			ft_free(1, &buffer2);
			return ;
		}
		p = -1;
		while (value[++p])
			*buffer = ft_strjoinchar_free(buffer, value[p]);
	}
	ft_free(2, &buffer2, &value);
	(*i)--;
}

char	*exp_env(char *line, t_env *head, t_data data, int *i)
{
	char	*value;
	char	*buffer;
	int		p;

	buffer = NULL;
	value = NULL;
	if (line[*i + 1] == '?')
	{
		(*i)++;
		value = ft_itoa(data.env_var);
		p = -1;
		while (value[++p])
			buffer = ft_strjoinchar_free(&buffer, value[p]);
		ft_free(1, &value);
	}
	else if (line[*i + 1] == ' ' || !line[*i + 1])
		buffer = ft_strjoinchar_free(&buffer, '$');
	else
		exp_env2(line, head, i, &buffer);
	return (buffer);
}
