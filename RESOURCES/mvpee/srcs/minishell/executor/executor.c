/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nechaara <nechaara@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/15 13:10:12 by mvpee             #+#    #+#             */
/*   Updated: 2024/04/13 01:26:33 by nechaara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/minishell.h"

static bool	init_executor(t_data *data)
{
	int	i;

	data->pid = malloc(sizeof(pid_t) * (data->nbr_cmd));
	if (!data->pid)
		return (true);
	data->pipefds = malloc(sizeof(int *) * (data->nbr_cmd));
	if (!data->pipefds)
		return (ft_free(1, &data->pid), true);
	i = -1;
	while (++i < data->nbr_cmd)
	{
		data->pipefds[i] = malloc(sizeof(int) * 2);
		if (!data->pipefds[i])
		{
			ft_free(1, &data->pid);
			ft_free_matrix(1, &data->pipefds);
			return (true);
		}
	}
	return (false);
}

static void	free_executor(t_parser *parser, t_data *data)
{
	int	i;

	i = -1;
	ft_free(1, &data->pid);
	while (++i < data->nbr_cmd)
		ft_free(1, &data->pipefds[i]);
	ft_free(1, &data->pipefds);
	free_parser(parser, *data);
}

static void	ft_waitpid(t_data *data)
{
	int	i;
	int	status;

	clean_fds(data);
	i = -1;
	while (++i < data->nbr_cmd)
		waitpid(data->pid[i], &status, 0);
	if (WIFEXITED(status))
		data->env_var = WEXITSTATUS(status);
	else if (g_flag == SIGQUIT)
	{
		g_flag = 0;
		data->env_var = COMMAND_INTERRUPTED_QUIT;
	}
	else
		data->env_var = COMMAND_INTERRUPTED;
}

static bool	ft_pipe(t_data *data)
{
	int	i;

	i = -1;
	while (++i < data->nbr_cmd)
	{
		if (pipe(data->pipefds[i]) == -1)
		{
			perror("pipe");
			clean_fds(data);
			return (false);
		}
	}
	return (true);
}

void	executor(t_env **head, t_data *data, t_parser *parser)
{
	if (data->flag)
	{
		data->flag = false;
		return ;
	}
	if (!parser || init_executor(data))
		return ;
	if (data->nbr_cmd == 1 && parser[0].ft_isspecial)
	{
		builtins(head, data, parser[0]);
		free_executor(parser, data);
		return ;
	}
	signal(SIGINT, SIG_IGN);
	if (ft_strcmp(parser[0].cmd[0], EXECUTABLE_NAME))
		signal(SIGQUIT, signal_quit);
	if (ft_pipe(data))
	{
		child_executor(head, data, parser);
		ft_waitpid(data);
	}
	signal(SIGINT, signal_handler);
	signal(SIGQUIT, SIG_IGN);
	free_executor(parser, data);
}
