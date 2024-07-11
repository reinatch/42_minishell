/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nechaara <nechaara@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 14:08:28 by nechaara          #+#    #+#             */
/*   Updated: 2024/04/13 01:24:10 by nechaara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/minishell.h"

static int	check_differences(char *str, char *bound)
{
	size_t	index;

	index = 0;
	while (str[index] && bound[index])
	{
		if (bound[index] < str[index])
			return (0);
		index++;
	}
	return (1);
}

static int	long_bound_check(char *str, char *bound)
{
	if (!str)
		return (0);
	else if (*str == '-' && !ft_strcmp(bound, LONG_MAX_STRING))
		return (1);
	if (ft_strlen(str) == ft_strlen(bound))
		return (check_differences(str, bound));
	else if (ft_strlen(str) < ft_strlen(bound))
		return (1);
	else if (ft_strlen(str) > ft_strlen(bound))
		return (0);
	return (1);
}

static void	error_message(size_t exit_code, char **split_args)
{
	if (exit_code == TOO_MANY_ARGUMENTS)
		ft_printf_fd(2, "exit: Too many arguments\n");
	else if (exit_code == NUMERICAL_ARGUMENT_NESCESSARY)
	{
		ft_printf_fd(2, "exit: %s", split_args[1]);
		ft_printf_fd(2, ": numeric argument required\n");
	}
}

static int	error_handler(char **split_args, size_t nbr_args,
		long *exit_code)
{
	if (nbr_args >= 2)
	{
		if (!ft_is_string_number(split_args[1]))
		{
			*exit_code = NUMERICAL_ARGUMENT_NESCESSARY;
			return (-1);
		}
		else if (!long_bound_check(split_args[1], LONG_MIN_STRING)
			|| !long_bound_check(split_args[1], LONG_MAX_STRING))
		{
			*exit_code = NUMERICAL_ARGUMENT_NESCESSARY;
			return (-1);
		}
		else if (nbr_args > 2)
		{
			*exit_code = TOO_MANY_ARGUMENTS;
			return (-2);
		}
	}
	return (0);
}

void	ft_exit(t_env *head, t_data *data, char **split)
{
	size_t	number_of_args;
	long	exit_code;
	int		error_handler_status;

	exit_code = 0;
	number_of_args = ft_splitlen((const char **)split);
	error_handler_status = error_handler(split, number_of_args, &exit_code);
	if (error_handler_status == 0)
	{
		if (number_of_args == 1)
			exit_code = data->env_var;
		else if (number_of_args == 2)
			exit_code = (ft_strtol(split[1], NULL, 10) % 256);
	}
	ft_printf("exit\n");
	if (error_handler_status == -1 || error_handler_status == -2)
		error_message(exit_code, split);
	if (error_handler_status == 0 || error_handler_status == -1)
	{
		ft_free_matrix(1, &split);
		free_env_list(head);
		exit(exit_code);
	}
	else
		data->env_var = exit_code;
}
