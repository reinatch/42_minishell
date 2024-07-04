/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amenses- <amenses-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/26 22:41:34 by amenses-          #+#    #+#             */
/*   Updated: 2023/03/05 18:08:22 by amenses-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

extern int	g_exit_status; // extern long long

static int	verify_num_arg(char *arg)
{
	int	i;

	i = 0;
	while (arg[i])
	{
		if (!ft_isdigit(arg[i]))
		{
			ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
			ft_putstr_fd(arg, STDERR_FILENO);
			ft_putstr_fd(": numeric argument required\n", STDERR_FILENO);
			return (0);
		}
		i++;
	}
	return (1);
}

long long	atoll_check(const char *nptr)
{
	unsigned long long	res;
	int					sign;

	sign = 1;
	res = 0;
	if (*nptr == '+' || *nptr == '-')
	{
		if (*nptr == '-')
			sign = -1;
		nptr++;
	}
	if (ft_strlen(nptr) > 19)
		return (-1);
	while ((*nptr >= 48 && *nptr <= 57) && *nptr)
	{
		res = res * 10 + *nptr - '0';
		nptr++;
	}
	if (res == 0)
		return (res);
	if ((sign > 0 && res > LLONG_MAX) || (sign < 0 && res - 1 >= LLONG_MAX))
		return (-1);
	if (res == 1 && sign < 0)
		return (255);
	return (res * sign);
}

int	get_exit_status(char *arg)
{
	long long	status;

	status = atoll_check(arg);
	if (status == -1)
	{
		ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
		ft_putstr_fd(arg, STDERR_FILENO);
		ft_putstr_fd(": numeric argument required\n", STDERR_FILENO);
		return (2);
	}
	if (status > 255)
		return (status % 256);
	if (status < 0)
		return (256 - ((-status) % 256));
	return (status);
}

int	ft_exit(t_app *self, char **args) //  review inputs ! everything should be destroyable
{
	int	status;

	(void)self;
	status = g_exit_status; // as per the man, default should be the previous exit status in storage, 0 if none
	ft_putstr_fd("exit\n", STDOUT_FILENO);
	if (args[1] && verify_num_arg(args[1]) == 0)
		status = 2;
	else if (args[1] && verify_num_arg(args[1]) == 1)
	{
		if (args[2])
		{
			ft_putstr_fd("minishell: exit: too many arguments\n", STDERR_FILENO);
			g_exit_status = 127; // set exit status in global
			return (g_exit_status); // exit instead of return? (bash does not actually exit in this case, but sets the exit status to 1)
		}
		else
			status = get_exit_status(args[1]);
	}
	// terminate(all) function // free all the freeables and clear history!
	rl_clear_history(); // clear history can be incorporated in terminate() is it necessary tho?
	g_exit_status = status;
	exit(g_exit_status); // not status but current exit status in the global var
	return (g_exit_status);
}

/* int	main(int argc, char **argv, char **envp)
{
	(void)argc;
	(void)envp;
	printf("status=%d\n", ft_exit(NULL, argv + 1));
	return (EXIT_SUCCESS);
} */
