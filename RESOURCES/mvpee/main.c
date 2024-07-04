/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nechaara <nechaara@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 21:37:59 by mvpee             #+#    #+#             */
/*   Updated: 2024/04/13 01:25:56 by nechaara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/minishell.h"

int	g_flag;

static void	print_welcome_message(t_env *head)
{
	t_env	*target_node;

	target_node = find_key(head, "SHLVL");
	ft_printf(RED "\n\n	███╗   ███╗██╗███╗   ██╗██╗" RED \
	BOLD "██╗  ██╗███████╗██╗     ██╗     \n" RESET);
	ft_printf(YELLOW "	████╗ ████║██║████╗  ██║██║" RED \
	BOLD "██║  ██║██╔════╝██║     ██║     \n" RESET);
	ft_printf(GREEN "	██╔████╔██║██║██╔██╗ ██║██║" RED \
	BOLD "███████║█████╗  ██║     ██║     \n" RESET);
	ft_printf(BLUE "	██║╚██╔╝██║██║██║╚██╗██║██║" RED \
	BOLD "██╔══██║██╔══╝  ██║     ██║     \n" RESET);
	ft_printf(MAGENTA "	██║ ╚═╝ ██║██║██║ ╚████║██║" RED \
	BOLD "██║  ██║███████╗███████╗███████╗\n" RESET);
	ft_printf(CYAN "	╚═╝     ╚═╝╚═╝╚═╝  ╚═══╝╚═╝" RED \
	BOLD "╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝\n\n" RESET);
	if (target_node)
		if (ft_strcmp(target_node->value, ""))
			ft_printf("SHLVL=%s\n", find_key(head, "SHLVL")->value);
}

char	*get_str_readline(void)
{
	char	*temp;
	char	*str_pwd;
	char	*str_readline;
	char	buffer[500];

	temp = ft_strjoin(YELLOW BOLD, getcwd(buffer, 500));
	str_pwd = ft_strjoin(temp, RESET);
	ft_free(1, &temp);
	temp = ft_strjoin(RED BOLD "minihell " RESET, str_pwd);
	str_readline = ft_strjoin(temp, "$ ");
	ft_free(2, &str_pwd, &temp);
	if (!str_readline)
	{
		if (chdir(getenv("HOME")) == -1)
			chdir("/");
		return (get_str_readline());
	}
	return (str_readline);
}

static void	init_all(t_env **head, t_data *data, char **envs)
{
	data->env_var = 0;
	data->flag = false;
	*head = env_init(envs);
	init_signal();
	print_welcome_message(*head);
}

int	main(int ac, char **argv, char **envs)
{
	t_env	*head;
	t_data	data;
	char	*line;
	char	*str_readline;

	(void)argv;
	(void)ac;
	init_all(&head, &data, envs);
	while (1)
	{
		str_readline = get_str_readline();
		line = readline(str_readline);
		ft_free(1, &str_readline);
		if (!line)
			break ;
		if (ft_strcmp(line, "\0"))
		{
			add_history(line);
			executor(&head, &data, ft_parser(ft_lexer(ft_strtrim(line, " "), \
						&data, head), &data, head));
		}
		ft_free(1, &line);
	}
	rl_clear_history();
	return (free_env_list(head), ft_printf("exit\n"), data.env_var);
}
