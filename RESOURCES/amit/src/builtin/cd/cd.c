/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amenses- <amenses-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/27 01:46:06 by amenses-          #+#    #+#             */
/*   Updated: 2023/03/05 18:00:02 by amenses-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

extern int	g_exit_status; // extern

static int count_args(char **args)
{
	int i;

	i = 0;
	while (args[i])
		i++;
	return (i);
}

static char    *get_env_value(t_env_list *env_list, char *key)
{
	t_env_list	*tmp;

	tmp = env_list;
	while (env_list)
	{
		if (ft_strncmp(env_list->key, key, ft_strlen(key) + 1) == 0)
			return (env_list->value);
		env_list = env_list->next;
	}
	env_list = tmp;
	return (NULL);
}

static int set_pwd(t_env_list *env_list, char *oldpwd)
{
	char		*pwd;
	t_env_list	*tmp;

	pwd = getcwd(NULL, 0);
	if (pwd == NULL) // if it arrived here, it mens chdir worked, which means pwd had been set correctly, so no failing case
		return (EXIT_FAILURE);
	tmp = env_list;
	while (env_list)
	{
		if (ft_strncmp(env_list->key, "OLDPWD", 7) == 0)
		{
			free(env_list->value);
			env_list->value = oldpwd;
		}
		if (ft_strncmp(env_list->key, "PWD", 4) == 0)
		{
			free(env_list->value);
			env_list->value = pwd;
		}
		env_list = env_list->next;
	}
	env_list = tmp;
	return (EXIT_SUCCESS);
}

static int	chto_home(t_env_list *env_list, char *oldpwd)
{
	char	*home;

	home = get_env_value(env_list, "HOME");
	if (home == NULL)
	{
		ft_putstr_fd("minishell: cd: HOME not set\n", STDERR_FILENO);
		g_exit_status = 2;
		return (g_exit_status);
	}
	if (chdir(home) != EXIT_SUCCESS)
	{
		ft_putstr_fd("minishell: cd: ", STDERR_FILENO);
		ft_putstr_fd(home, STDERR_FILENO);
		ft_putstr_fd(": No such file or directory\n", STDERR_FILENO);
		g_exit_status = 1;
		return (g_exit_status);
	}
	set_pwd(env_list, oldpwd);
	g_exit_status = 0;
	return (g_exit_status);
}

int ft_cd(t_app *self, char **args) // not considering paths in CDPATH which does not seem to be available in these pcs bash
{
	char	*oldpwd;

	oldpwd = ft_strdup(get_env_value(self->env_list, "PWD"));
	if (count_args(args) > 2)
	{
		ft_putstr_fd("minishell: cd: too many arguments\n", STDERR_FILENO);
		g_exit_status = 1;
		return (g_exit_status);
	}
	if (args[1] == NULL)
		chto_home(self->env_list, oldpwd);
	else
	{
		if (chdir(args[1]) != EXIT_SUCCESS)
		{
			ft_putstr_fd("minishell: cd: ", STDERR_FILENO);
			ft_putstr_fd(args[1], STDERR_FILENO);
			ft_putstr_fd(": No such file or directory\n", STDERR_FILENO);
			g_exit_status = 1;
			return (g_exit_status);
		}
		set_pwd(self->env_list, oldpwd);
		g_exit_status = 0;
	}
	return (g_exit_status);
}

/* static void	export_display(t_env_list *env_list)
{
	t_env_list	*tmp;

	tmp = env_list;
	while (tmp)
	{
		if (tmp->value == NULL)
			printf("declare -x %s\n", tmp->key);
		else
			printf("declare -x %s=\"%s\"\n", tmp->key, tmp->value);
		tmp = tmp->next;
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_app	*app;

	(void)argc;
	(void)argv;
	app = malloc(sizeof(t_app));
	fill_env_list(app, envp); // conditional jump/uninitialized value in push_front
	ft_cd(app, argv + 1);
	// printf("status=%d(%s)\n", ft_cd(app, argv + 1), argv[1]);
	export_display(app->env_list);
	// printf("%s\n", getcwd(NULL, 0));
	printf("exit_status=%d\n", g_exit_status);
	free_env_list(app);
	free(app);
	return (0);
} */
