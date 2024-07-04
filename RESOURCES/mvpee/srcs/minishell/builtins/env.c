/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nechaara <nechaara@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/13 13:37:37 by mvpee             #+#    #+#             */
/*   Updated: 2024/03/11 17:28:28 by nechaara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/minishell.h"

void	ft_env(t_data *data, t_env *head, char **split)
{
	if (split[1])
	{
		ft_printf_fd(2, "env: '%s' No such file or directory\n", split[1]);
		data->env_var = COMMAND_NOT_FOUND;
	}
	else
	{
		while (head)
		{
			if (head->value && ft_strcmp(head->key, "_"))
				ft_printf("%s=%s\n", head->key, head->value);
			head = head->next;
		}
		ft_printf("_=/usr/bin/env\n");
		data->env_var = SUCCESSFUL_COMMAND;
	}
}
