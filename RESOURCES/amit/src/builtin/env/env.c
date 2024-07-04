/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amenses- <amenses-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/25 23:09:01 by amenses-          #+#    #+#             */
/*   Updated: 2023/03/05 18:07:47 by amenses-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/minishell.h"

extern int	g_exit_status; // extern

int	ft_env(t_app *self, char **args)
{
	t_env_list	*tmp;

	if (args[1])
	{
		ft_putstr_fd("env: ", STDERR_FILENO);
		ft_putstr_fd(args[1], STDERR_FILENO);
		ft_putstr_fd(": No such file or directory\n", STDERR_FILENO);
		g_exit_status = 127;
		return (g_exit_status);
	}
	tmp = self->env_list;
	while (tmp)
	{
		ft_putstr_fd(tmp->key, STDOUT_FILENO);
		ft_putstr_fd("=", STDOUT_FILENO);
		ft_putstr_fd(tmp->value, STDOUT_FILENO);
		ft_putstr_fd("\n", STDOUT_FILENO);
		tmp = tmp->next;
	}
	g_exit_status = 0;
	return (g_exit_status);
}

/* int	main(int argc, char **argv, char **envp)
{
	t_app	*app;
	int		status;

	(void)argc;
	app = malloc(sizeof(t_app));
	fill_env_list(app, envp);
	status = ft_env(&app, argv + 1);
	printf("(status: %d)\n", status);
	free_env_list(app);
	free(app);
	return (EXIT_SUCCESS);
} */