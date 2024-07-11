/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvpee <mvpee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/12 17:08:04 by nechaara          #+#    #+#             */
/*   Updated: 2024/03/17 20:13:49 by mvpee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/minishell.h"

static void	ft_switch_cd(t_env *env, char *value)
{
	if (!env || !value)
		return ;
	ft_free(1, &env->value);
	env->value = ft_strdup(value);
}

static int	ft_cd_others(t_env *head, char **split)
{
	char	buffer[500];

	if (chdir(split[1]) != 0)
		return (ft_printf_fd(2, "cd: "), perror(split[1]), 1);
	if (find_key(head, "OLDPWD"))
	{
		if (find_key(head, "PWD"))
			ft_switch_cd(find_key(head, "OLDPWD"), find_key(head, \
					"PWD")->value);
		else
		{
			ft_free(1, &find_key(head, "OLDPWD")->value);
			find_key(head, "OLDPWD")->value = ft_strdup("");
		}
	}
	if (find_key(head, "PWD"))
		ft_switch_cd(find_key(head, "PWD"), getcwd(buffer, 500));
	return (0);
}

static int	ft_cd_with_minus(t_env *head, char **split)
{
	char	buffer[500];

	if (!find_key(head, "OLDPWD"))
		return (ft_printf_fd(2, "%s: OLDPWD not set\n", split[0]), 1);
	if (!ft_strcmp(find_key(head, "OLDPWD")->value, ""))
		return (ft_printf_fd(2, "%s: OLDPWD not set\n", split[0]), 1);
	if (chdir(find_key(head, "OLDPWD")->value) != 0)
		return (ft_printf_fd(2, "cd: %s: No such file or directory\n", \
				find_key(head, "OLDPWD")->value), 1);
	if (find_key(head, "OLDPWD"))
	{
		if (find_key(head, "PWD"))
			ft_switch_cd(find_key(head, "OLDPWD"), find_key(head,
					"PWD")->value);
		else
		{
			ft_free(1, &find_key(head, "OLDPWD")->value);
			find_key(head, "OLDPWD")->value = ft_strdup("");
		}
	}
	if (find_key(head, "PWD"))
		ft_switch_cd(find_key(head, "PWD"), getcwd(buffer, 500));
	return (0);
}

static int	ft_cd_with_no_arguments(t_env *head, char **split)
{
	char	buffer[500];

	if (find_key(head, "HOME"))
	{
		if (chdir(find_key(head, "HOME")->value) != 0)
			return (perror(split[0]), 1);
	}
	else
		return (ft_printf_fd(2, "cd: HOME not set\n"));
	if (find_key(head, "OLDPWD"))
	{
		if (find_key(head, "PWD"))
			ft_switch_cd(find_key(head, "OLDPWD"), find_key(head, \
					"PWD")->value);
		else
		{
			ft_free(1, &find_key(head, "OLDPWD")->value);
			find_key(head, "OLDPWD")->value = ft_strdup("");
		}
	}
	if (find_key(head, "PWD"))
		ft_switch_cd(find_key(head, "PWD"), getcwd(buffer, 500));
	return (0);
}

void	ft_cd(t_env *head, t_data *data, char **split)
{
	if (ft_splitlen((const char **)split) > 2)
	{
		ft_printf_fd(2, "%s: too many arguments\n", split[0]);
		data->env_var = TOO_MANY_ARGUMENTS;
	}
	else if (!split[1])
		data->env_var = ft_cd_with_no_arguments(head, split);
	else if (!ft_strcmp(split[1], "-"))
		data->env_var = ft_cd_with_minus(head, split);
	else
		data->env_var = ft_cd_others(head, split);
}
