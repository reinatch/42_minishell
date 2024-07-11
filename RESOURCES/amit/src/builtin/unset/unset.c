/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amenses- <amenses-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/25 20:02:37 by amenses-          #+#    #+#             */
/*   Updated: 2023/03/05 23:21:43 by amenses-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/minishell.h"

extern int g_exit_status; // extern

static int	isalphanum_(char *arg)
{
	int	i;

	i = 0;
	while (arg[i])
	{
		if (!ft_isalnum(arg[i]) && arg[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

static int	check_args(char **args)
{
	int	i;
	int	status;

	i = 1;
	status = 1;
	while (args[i])
	{
		if (!isalphanum_(args[i]))
		{
			ft_putstr_fd("minishell: unset: `", STDERR_FILENO);
			ft_putstr_fd(args[i], STDERR_FILENO);
			ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
			status = 0;
		}
		i++;
	}
	return (status);
}

int	ft_unset(t_app *self, char **args)
{
	int	i;

	if (check_args(args) == 0)
	{
		g_exit_status = 1;
		return (g_exit_status);
	}
	i = 1;
	while (args[i])
	{
		remove_list_item_by_key(self, args[i]);
		i++;
	}
	g_exit_status = 0;
	return (g_exit_status);
}

/* void	print_env_list(t_app *self)
{
	t_env_list	*tmp;

	tmp = self->env_list;
	while (tmp)
	{
		printf("%s=%s\n", tmp->key, tmp->value);
		tmp = tmp->next;
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_app	*app;
	int		status;

	(void)argc;
	app = malloc(sizeof(t_app));
	fill_env_list(app, envp);
	print_env_list(app);
	printf("====================\n");
	status = ft_unset(&app, argv + 1);
	// print_env_list(app);
	printf("(status: %d)\n", status);
	return (status);
} */
