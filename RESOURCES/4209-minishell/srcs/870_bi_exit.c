/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   870_bi_exit.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoda-s <antoda-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 00:10:23 by antoda-s          #+#    #+#             */
/*   Updated: 2024/04/03 23:01:39 by antoda-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/// @brief 		Exit function with error message
/// @param s 	Script structure with commans and args
/// @param arg 	Argument to be checked
/// @param msg 	Message to be shown
/// @param sys 	System error
void	exit_go(t_script *s, char *arg, char *msg, int sys)
{
	ft_putstr_fd("exit\n", 1);
	if (sys > 0 && (arg || msg))
	{
		ft_putstr_fd("minishell: exit: ", 2);
		if (arg)
			ft_putstr_fd(arg, 2);
		if (msg)
			ft_putstr_fd(msg, 2);
	}
	errno = sys;
	free_exit(s, sys);
	exit(sys);
}

/// @brief 			Builtin exit command limited to 9223372036854775807
/// @param args		Builtin command arguments
/// @return			exit status or ERROR
int	bi_exit(t_script *s, int n)
{
	char	*trim;

	trim = NULL;
	if (n > 0)
		return (SUCCESS);
	if (s->cmds[n].argc == 1)
		exit_go(s, NULL, NULL, 0);
	else if (s->cmds[n].argc > 2)
		g_exit_status = error_exit_args("too many arguments\n", 1);
	else if (s->cmds[n].argc == 2)
	{
		if (!ft_isnum(s->cmds[n].argv[1]))
			exit_go(s, s->cmds[n].argv[1], ": numeric argument required\n", 2);
		trim = ft_strtrim_leading(s->cmds[n].argv[1], "0");
		free (s->cmds[n].argv[1]);
		s->cmds[n].argv[1] = trim;
		if (ft_atoll(trim) > __LONG_LONG_MAX__
			|| ft_atoll(trim) < -9223372036854775808.0)
			exit_go(s, trim, ": numeric argument required\n", 2);
		else
			exit_go(s, NULL, NULL, ft_atoi(trim) % 256);
	}
	return (0);
}
