/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_expand_home.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvpee <mvpee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/08 15:27:04 by mvpee             #+#    #+#             */
/*   Updated: 2024/03/13 11:27:25 by mvpee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../../includes/minishell.h"

static void	ft_expand_home2(int *i, char **buffer)
{
	char	*value;
	int		p;

	value = getenv("HOME");
	if (value)
	{
		p = -1;
		while (value[++p])
			*buffer = ft_strjoinchar_free(buffer, value[p]);
	}
	(*i)++;
}

void	ft_expand_home(char *line, int *i, char **buffer)
{
	char	*value;
	int		p;

	value = NULL;
	if (*i == 0)
	{
		*buffer = NULL;
		if (line[*i] == '~' && (line[*i + 1] == ' ' || !line[*i + 1]))
		{
			value = getenv("HOME");
			if (value)
			{
				p = -1;
				while (value[++p])
					*buffer = ft_strjoinchar_free(buffer, value[p]);
			}
			(*i)++;
		}
	}
	else if (line[*i] == '~' && line[*i - 1] == ' ' && (line[*i + 1] == ' ' \
			|| !line[*i + 1]))
		ft_expand_home2(i, buffer);
}
