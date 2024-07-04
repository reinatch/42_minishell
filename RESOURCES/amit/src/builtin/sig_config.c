/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sig_config.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amenses- <amenses-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/03 20:07:47 by amenses-          #+#    #+#             */
/*   Updated: 2023/03/05 23:12:35 by amenses-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

extern int	g_exit_status;

static void	ctrl_c(int sig)
{
	if (sig == SIGINT)
	{
		ft_putchar_fd('\n', STDOUT_FILENO);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
		g_exit_status = 128 + SIGINT;
		// kill(0, SIGKILL); // kill all processes in the process group of the calling process
		exit(g_exit_status); // set system exit status
	}
}

int	sig_config(void)
// int	main()
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	sa_int.sa_handler = &ctrl_c;
	sa_quit.sa_handler = SIG_IGN;
	sigemptyset(&sa_int.sa_mask);
	sigaction(SIGINT, &sa_int, NULL);
	sigaction(SIGQUIT, &sa_quit, NULL);
	return (EXIT_SUCCESS);
}
