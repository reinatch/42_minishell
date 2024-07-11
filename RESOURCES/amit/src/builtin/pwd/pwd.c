/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amenses- <amenses-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/26 20:31:20 by amenses-          #+#    #+#             */
/*   Updated: 2023/03/03 20:04:29 by amenses-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

extern int g_exit_status; // extern

int	ft_pwd(t_app *self, char **args) // this function does not require inputs, review inputs
{
	char	*pwd;

	(void)self;
	(void)args;
	pwd = getcwd(NULL, 0);
	if (pwd == NULL)
	{
		perror("minishell: pwd");
		return (EXIT_FAILURE);
	}
	ft_putstr_fd(pwd, STDOUT_FILENO);
	ft_putstr_fd("\n", STDOUT_FILENO);
	free(pwd);
	g_exit_status = 0;
	return (g_exit_status);
}

/* int	main(int argc, char **argv, char **envp)
{
	t_app	*app;

	perror("minishell: pwd");
	(void)argc;
	app = malloc(sizeof(t_app));
	fill_env_list(app, envp);
	if (argv[1] && ft_strncmp(argv[1], "pwd", 4) == 0)
		ft_pwd(app, argv + 2);
	else
		return (0);
	free_env_list(app);
	free(app);
	return (EXIT_SUCCESS);
} */