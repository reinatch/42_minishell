/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amenses- <amenses-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/26 00:22:40 by amenses-          #+#    #+#             */
/*   Updated: 2023/03/06 01:35:10 by amenses-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

extern int	g_exit_status; // extern

/* static void	print_env_list(t_env_list *self)
{
	t_env_list	*tmp;

	tmp = self;
	while (tmp)
	{
		printf("key=%s, value=%s\n", tmp->key, tmp->value);
		tmp = tmp->next;
	}
} */

static t_env_list	*init(char *key, char *value) // build general new item function
{
	t_env_list	*new;

	new = malloc(sizeof(t_env_list));
	new->key = key;
	new->value = value;
	new->next = NULL;
	return (new);
}

static void	push_front(t_app *self, t_env_list *new) // build general push front function
{
	if (new == NULL)
		return ;
	if (self->env_list == NULL)
	{
		self->env_list = new;
		return ;
	}
	new->next = self->env_list;
	self->env_list = new;
}

static int	validate_key(char *arg)
{
	int		i;
	char	*del;
	int		index;

	del = ft_strchr(arg, '=');
	if (del == NULL)
		index = ft_strlen(arg);
	else
		index = (int)(del - arg);
	if (index == 0)
		return (0);
	if (arg[0] == '*' && (arg[1] != '\0' || arg[1] != ';'))
		return (0);
	else if (!ft_isalpha(arg[0]) && arg[0] != '_')
		return (0);
	i = 0;
	while (i < index)
	{
		if (!ft_isalnum(arg[i]) && arg[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

static void	add_envlist_item(t_app **self, char *arg)
{
	t_env_list	*new;
	char		*del;
	int			index;

	del = ft_strchr(arg, '=');
	if (del == NULL)
		index = ft_strlen(arg);
	else
		index = (int)(del - arg);
	if (del == NULL)
		new = init(ft_substr(arg, 0, index), NULL);
	else
		new = init(ft_substr(arg, 0, index), \
			ft_substr(arg, index + 1, ft_strlen(arg)));
	push_front(*self, new);
}

static void	export_display(t_env_list *env_list)
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

static void	sorted_display(t_app *self_dup)
{
	t_env_list	*tmp[2];
	char		*tmp_key_val[2];

	tmp[0] = self_dup->env_list;
	while (tmp[0])
	{
		tmp[1] = tmp[0]->next;
		while (tmp[1])
		{
			if (ft_strncmp(tmp[0]->key, tmp[1]->key, \
				ft_strlen(tmp[0]->key) + ft_strlen(tmp[1]->key)) > 0)
			{
				tmp_key_val[0] = tmp[0]->key;
				tmp_key_val[1] = tmp[0]->value;
				tmp[0]->key = tmp[1]->key;
				tmp[0]->value = tmp[1]->value;
				tmp[1]->key = tmp_key_val[0];
				tmp[1]->value = tmp_key_val[1];
			}
			tmp[1] = tmp[1]->next;
		}
		tmp[0] = tmp[0]->next;
	}
	export_display(self_dup->env_list);
	// free_env_list(self_dup); // double free error, investigate!
}

static t_app	*env_list_dup(t_env_list *env_list)
{
	t_app		*new;
	t_env_list	*tmp;

	tmp = env_list;
	new = malloc(sizeof(t_app));
	ft_bzero(new, sizeof(t_app));
	while (tmp)
	{
		if (tmp->value == NULL)
			push_front(new, init(ft_strdup(tmp->key), NULL));
		else
			push_front(new, init(ft_strdup(tmp->key), ft_strdup(tmp->value)));
		tmp = tmp->next;
	}
	// print_env_list(new->env_list);
	// exit(0);
	return (new);
}

int	ft_export(t_app *self, char **args)
{
	int	i;
	int	status;

	i = 1;
	status = EXIT_SUCCESS;
	if (args[1] == NULL) // confirm if this is the way the input is passed
		sorted_display(env_list_dup(self->env_list));
	while (args[i])
	{
		if (validate_key(args[i]))
			add_envlist_item(&self, args[i]);
		else
		{
			ft_putstr_fd("minishell: export: `", STDERR_FILENO);
			ft_putstr_fd(args[i], STDERR_FILENO);
			ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
			status = EXIT_FAILURE;
		}
		i++;
	}
	g_exit_status = status;
	return (g_exit_status);
}

/* int main(int argc, char **argv, char **envp)
{
	t_app	*app;

	(void)argc;
	app = malloc(sizeof(t_app));
	fill_env_list(app, envp);
	printf("status=%d\n", ft_export(app, argv + 1));
	// printf("%s\n", argv[1]);
	ft_export(app, argv + 2);
	return (0);
} */
