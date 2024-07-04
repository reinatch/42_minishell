/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvan-pee <mvan-pee@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/15 13:06:36 by mvpee             #+#    #+#             */
/*   Updated: 2024/03/15 18:25:05 by mvan-pee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/minishell.h"

bool	ft_isspecial(t_parser parser)
{
	if (!ft_strcmp("unset", parser.cmd[0]) || !ft_strcmp("cd", parser.cmd[0])
		|| !ft_strcmp("exit", parser.cmd[0]))
		return (true);
	if (!ft_strcmp("export", parser.cmd[0]) && parser.cmd[1])
		return (true);
	return (false);
}

bool	isbuiltins(char *line)
{
	if (!line)
		return (false);
	if (!ft_strcmp("echo", line) || !ft_strcmp("pwd", line) \
		|| !ft_strcmp("env", line) || !ft_strcmp("unset", line) \
		|| !ft_strcmp("export", line) || !ft_strcmp("cd", line) \
		|| !ft_strcmp("exit", line))
		return (true);
	return (false);
}

char	*builtins(t_env **head, t_data *data, t_parser parser)
{
	if (!ft_strcmp(parser.cmd[0], "echo"))
		ft_echo(data, parser.cmd);
	else if (!ft_strcmp(parser.cmd[0], "env"))
		ft_env(data, *head, parser.cmd);
	else if (!ft_strcmp(parser.cmd[0], "pwd"))
		ft_pwd(data);
	else if (!ft_strcmp(parser.cmd[0], "unset"))
		ft_unset(head, data, parser.cmd);
	else if (!ft_strcmp(parser.cmd[0], "export"))
		ft_export(head, data, parser.cmd);
	else if (!ft_strcmp(parser.cmd[0], "cd"))
		ft_cd(*head, data, parser.cmd);
	else if (!ft_strcmp(parser.cmd[0], "exit"))
		ft_exit(*head, data, parser.cmd);
	return (NULL);
}
