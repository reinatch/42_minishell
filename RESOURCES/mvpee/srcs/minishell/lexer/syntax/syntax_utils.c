/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvpee <mvpee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/13 08:56:30 by mvpee             #+#    #+#             */
/*   Updated: 2024/03/16 18:10:04 by mvpee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../../includes/minishell.h"

bool	check_after(char *line, int i)
{
	int	j;

	j = i;
	while (line[++j])
	{
		while (line[j] == ' ')
			j++;
		if (line[j] == '|')
			return (true);
		else if (ft_isprint(line[j]))
			return (false);
		else if (!line[j])
			return (true);
	}
	return (true);
}

bool	check_before(char *line, int i)
{
	int	j;

	j = i;
	if (j == 0)
		return (true);
	while (line[--j])
	{
		while (line[j] == ' ')
			j--;
		if (ft_isprint(line[j]))
			return (false);
		else if (line[j] == '|')
			return (true);
		else if (!line[j])
			return (true);
		if (j == 0)
			return (true);
	}
	return (false);
}
