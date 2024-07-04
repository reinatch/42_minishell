/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child_executor.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvan-pee <mvan-pee@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/07 22:43:58 by mvpee             #+#    #+#             */
/*   Updated: 2024/03/18 12:01:11 by mvan-pee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/minishell.h"

static void	ft_pipe(t_data *data, int i)
{
	int	j;

	j = -1;
	if (i < data->nbr_cmd - 1)
		dup2(data->pipefds[i][1], STDOUT_FILENO);
	if (i > 0)
		dup2(data->pipefds[i - 1][0], STDIN_FILENO);
	while (++j < data->nbr_cmd)
	{
		close(data->pipefds[j][0]);
		close(data->pipefds[j][1]);
	}
}

static void	ft_redirection(t_parser parser)
{
	if (parser.input != -1)
	{
		dup2(parser.input, STDIN_FILENO);
		close(parser.input);
	}
	if (parser.output != -1)
	{
		dup2(parser.output, STDOUT_FILENO);
		close(parser.output);
	}
}

static void	ft_execve_check(t_parser parser)
{
	if (parser.path == NULL && !parser.isbuiltins)
	{
		if (ft_ischarin('/', parser.cmd[0]))
			ft_printf_fd(2, "%s: No such file or directory\n", parser.cmd[0]);
		else
			ft_printf_fd(2, "%s: command not found\n", parser.cmd[0]);
		exit(COMMAND_NOT_FOUND);
	}
	if (parser.flag)
		exit(1);
}

static void	ft_execve(t_parser parser, t_env **head, t_data *data, int i)
{
	char	**env;

	env = env_to_tab(*head);
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	ft_execve_check(parser);
	ft_pipe(data, i);
	ft_redirection(parser);
	if (parser.isbuiltins)
	{
		dup2(data->pipefds[i][1], STDOUT_FILENO);
		close(data->pipefds[i][1]);
		close(data->pipefds[i][0]);
		builtins(head, data, parser);
		exit(0);
	}
	execve(parser.path, parser.cmd, env);
	if (access(parser.cmd[0], X_OK) == 0)
		ft_printf_fd(2, "%s: is a directory\n", parser.cmd[0]);
	else
		perror(parser.cmd[0]);
	ft_free_matrix(1, &env);
	if (errno == EACCES)
		exit(MISSING_RIGHTS);
	exit(COMMAND_NOT_FOUND);
}

void	child_executor(t_env **head, t_data *data, t_parser *parser)
{
	int	i;

	i = -1;
	while (++i < data->nbr_cmd)
	{
		data->pid[i] = fork();
		if (data->pid[i] < 0)
		{
			perror("fork");
			return ;
		}
		if (data->pid[i] == 0)
		{
			if (parser[i].cmd == NULL)
			{
				if (parser[i].input != -1 || parser[i].output != -1)
					exit(0);
				exit(1);
			}
			ft_execve(parser[i], head, data, i);
		}
	}
}
