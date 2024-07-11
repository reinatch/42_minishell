/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvpee <mvpee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/14 10:59:48 by mvan-pee          #+#    #+#             */
/*   Updated: 2024/03/13 11:23:15 by mvpee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/minishell.h"

void	ft_pwd(t_data *data)
{
	char	buffer[500];

	data->env_var = SUCCESSFUL_COMMAND;
	ft_printf("%s\n", getcwd(buffer, 500));
}
