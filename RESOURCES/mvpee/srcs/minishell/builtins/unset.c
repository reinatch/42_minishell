/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nechaara <nechaara@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/14 14:11:33 by mvan-pee          #+#    #+#             */
/*   Updated: 2024/03/11 17:28:46 by nechaara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/minishell.h"

void	ft_unset(t_env **head, t_data *data, char **split)
{
	int	index;

	if (!split)
		return ;
	if (ft_splitlen((const char **)split) > 1)
	{
		index = 1;
		while (split[index])
		{
			env_remove_entry(head, split[index]);
			index++;
		}
	}
	data->env_var = SUCCESSFUL_COMMAND;
}
