/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amenses- <amenses-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/04 19:32:46 by amitcul           #+#    #+#             */
/*   Updated: 2023/06/08 20:43:48 by amenses-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

#define PROMPT "$>"

int	g_exit_status = 0;

int init_app(t_app *app)
{
	app->env_list = NULL;
	app->input = NULL;
	app->lexer_tokens = NULL;
	app->commands_list = NULL;
	app->pipes_count = 0;
	app->reset = false;
	app->pid = NULL;
	app->heredoc = false;
	// global_variable_or_struct_init???
//	parse_envp(app);
	// init_signals();
	return (1);
}

int loop(t_app *app);
int reset(t_app *app)
{
	/*
	reset and clear all structs
	*/
	loop(app);
	return (EXIT_SUCCESS);
}

void	ft_splitfree(char **str)
{
	int i;

	i = -1;
	while (str[++i])
		free(str[i]);
	free(str);
}

// my
void	print_commands_list(t_command *commands_list)
{
	t_command *tmp;
	t_lexer_token *token;

	tmp = commands_list;
	printf("\n");
	while (tmp)
	{
		token = tmp->redirs;
		printf("command: %s\n", tmp->str[0]);
		printf("args:\n");
		for (int i = 1; tmp->str[i]; i++)
			printf("_%s\n", tmp->str[i]);
		printf("heredoc: %s\n", tmp->heredoc);
		printf("redirs___\n");
		while (token)
		{
			printf("_str: %s\n", token->str);
			printf("_token_type: %d\n", token->token_type);
			token = token->next;
		}
		printf("\n");
		tmp = tmp->next;
	}
}

int	red_out(t_command *cmd, t_lexer_token *red)
{
	int	fd;

	(void)cmd;
	fd = open(red->str, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(red->str, STDERR_FILENO);
		perror(": ");
		// ft_putendl_fd(": Permission denied", STDERR_FILENO);
		// EXITSTATUS = 1
	}
	return (fd);
}

int	red_append(t_command *cmd, t_lexer_token *red)
{
	int	fd;

	(void)cmd;
	fd = open(red->str, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(red->str, STDERR_FILENO);
		perror(": ");
		// EXITSTATUS = 1
	}
	return (fd);
}

int	red_in(t_command *cmd, t_lexer_token *red)
{
	int	fd;

	(void)cmd;
	fd = open(red->str, O_RDONLY);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(red->str, STDERR_FILENO);
		perror(": ");
		// EXITSTATUS = 1
	}
	return (fd);
}

int	red_heredoc(t_command *cmd, t_lexer_token *red)
{
	char	*l;
	int		fd;

	(void)cmd;
	fd = open("tmp_XmXiXnXiXsXhXeXlXl", O_CREAT | O_RDWR | O_TRUNC, 0644);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(red->str, STDERR_FILENO);
		perror(": ");
		// EXITSTATUS = 1
		return (-1);
	}
	while (1)
	{
		l = readline(">");
		if (ft_strncmp(l, red->str, ft_strlen(red->str) + 1) == 0)
			break ;
		ft_putendl_fd(l, fd);
		// ft_putstr_fd(l, fd);
		// ft_putstr_fd("\n", fd);
		free(l);
	}
	free(l);
	close (fd);
	fd = open("tmp_XmXiXnXiXsXhXeXlXl", O_RDONLY);
	if (fd == -1)
		perror("minishell: tmp_XmXiXnXiXsXhXeXlXl: ");
	unlink("tmp_XmXiXnXiXsXhXeXlXl"); // confirm
		// EXITSTATUS = 1
	return (fd);
}

int	upd_fd(t_command *cmd, t_lexer_token *red, int (*f)(t_command*, t_lexer_token*), int io)
{
	if (cmd->fd[io] != io)
		close(cmd->fd[io]);
	cmd->fd[io] = f(cmd, red);
	printf("==fd[%d] = %d\n", io, cmd->fd[io]);
	return (cmd->fd[io]);
}

int	redirector(t_command *commands_list)
{
	t_lexer_token	*red;
	t_command		*cmd;

	cmd = commands_list;
	while (cmd)
	{
		red = cmd->redirs;
		cmd->fd[0] = STDIN_FILENO; // default 0 (might already have been initilized as 0)
		cmd->fd[1] = STDOUT_FILENO; // default 1
		while (red)
		{
			if (red->token_type == GREAT && upd_fd(cmd, red, &red_out, 1) == -1) // > (out)
				return (-1);
				// cmd->fd[1] = red_out(cmd, red);
			if (red->token_type == G_GREAT && upd_fd(cmd, red, &red_append, 1) == -1) // >> (append)
				return (-1);
			// 	cmd->fd[1] = red_append(cmd, red);
			if (red->token_type == LESS && upd_fd(cmd, red, &red_in, 0) == -1) // < (in)
				return (-1);
			// 	cmd->fd[0] = red_in(cmd, red);
			if (red->token_type == L_LESS && upd_fd(cmd, red, &red_heredoc, 0) == -1) // << (heredoc)
				return (-1);
			// 	cmd->fd[0] = red_heredoc(cmd, red);
			red = red->next;
		}
		cmd = cmd->next;
	}
	return (0);
}

void	print_commands_fd(t_command *cmd)
{
	t_command	*tmp;
	
	tmp = cmd;
	while (cmd)
	{
		printf("cmd: %s, ", cmd->str[0]);
		printf("fd[0]: %d, ", cmd->fd[0]);
		printf("fd[1]: %d\n", cmd->fd[1]);
		cmd = cmd->next;
	}
}

int	isbuiltin(char *cmd)
{
	if (ft_strncmp(cmd, "echo", 5) == 0)
		return (1);
	if (ft_strncmp(cmd, "cd", 3) == 0)
		return (1);
	if (ft_strncmp(cmd, "pwd", 4) == 0)
		return (1);
	if (ft_strncmp(cmd, "export", 7) == 0)
		return (1);
	if (ft_strncmp(cmd, "unset", 6) == 0)
		return (1);
	if (ft_strncmp(cmd, "env", 4) == 0)
		return (1);
	if (ft_strncmp(cmd, "exit", 5) == 0)
		return (1);
	return (0);
}

int	bi(t_command *cmd)
{
	// (void)cmd;
	if (ft_strncmp(cmd->str[0], "echo", 5) == 0)
		return (ft_echo(NULL, cmd->str));
	else if (ft_strncmp(cmd->str[0], "cd", 3) == 0)
		return (ft_cd(NULL, cmd->str));
	else if (ft_strncmp(cmd->str[0], "pwd", 4) == 0)
		return (ft_pwd(NULL, cmd->str));
	else if (ft_strncmp(cmd->str[0], "export", 7) == 0)
		return (ft_export(NULL, cmd->str));
	else if (ft_strncmp(cmd->str[0], "unset", 6) == 0)
		return (ft_unset(NULL, cmd->str));
	else if (ft_strncmp(cmd->str[0], "env", 4) == 0)
		return (ft_env(NULL, cmd->str));
	else if (ft_strncmp(cmd->str[0], "exit", 5) == 0)
		return (ft_exit(NULL, cmd->str));
	return (0);
}

int	exec_bi(t_command *cmd, int pp) // app npipes
{
	pid_t	pid;
	int		wstatus;

	wstatus = 0;
	if (pp != 0)
	{
		pid = fork();
		if (pid == 0)
			return (bi(cmd));
		else if (pid < 0)
			perror("minishell: fork: ");
		else
			waitpid(pid, &wstatus, 0);
	}
	else
		return (bi(cmd));
	return (WEXITSTATUS(wstatus));
}

int	exec_cmd(t_command *cmd, char **envp)
{
	pid_t	pid;
	// int		std[2];
	int		wstatus;

	wstatus = 0;
	// std[0] = dup(STDIN_FILENO);
	// std[1] = dup(STDOUT_FILENO);
	// dup2(cmd->fd[0], STDIN_FILENO);
	// if (cmd->fd[0] != STDIN_FILENO)
	// 	close(cmd->fd[0]);
	// dup2(cmd->fd[1], STDOUT_FILENO);
	// if (cmd->fd[1] != STDOUT_FILENO)
	// 	close(cmd->fd[1]);
	if (isbuiltin(cmd->str[0])) // check for no pipes
	{
		// wstatus = exec_bi(cmd, 0);
		// dup2(std[0], STDIN_FILENO);
		// dup2(std[1], STDOUT_FILENO);
		// close(std[0]);
		// close(std[1]);
		return (exec_bi(cmd, 0)); // npipes
	}
	else
	{
		// printf("cmd->args[0]=%s\n", cmd->args[0]);
		if (access(cmd->str[0], X_OK) == -1)
		{
			ft_putstr_fd("minishell: ", STDERR_FILENO);
			ft_putstr_fd(cmd->str[0], STDERR_FILENO);
			ft_putendl_fd(": permission denied", STDERR_FILENO);
			// perror(": ");
			return (126); // EXITSTATUS: 126
		}
		pid = fork();
		if (pid == 0)
			execve(cmd->str[0], cmd->str, envp);
		else if (pid < 0)
			perror("minishell: fork: ");
		else
			waitpid(pid, &wstatus, 0);
	}
	// dup2(std[0], STDIN_FILENO);
	// dup2(std[1], STDOUT_FILENO);
	// close(std[0]);
	// close(std[1]);
	return (WEXITSTATUS(wstatus));
}

void	upd_stdfd(t_command *commands_list, int std[2])
{
	t_command	*cmd;

	cmd = commands_list;
	while (cmd)
	{
		if (cmd->fd[0] == STDIN_FILENO)
			cmd->fd[0] = std[0];
		if (cmd->fd[1] == STDOUT_FILENO)
			cmd->fd[1] = std[1];
		cmd = cmd->next;
	}
}

int	exec_ppx(t_command *commands_list, char **envp)
{
	int			pp[2];
	int			std[2];
	t_command	*cmd;
	int			status;

	cmd = commands_list;
	std[0] = dup(STDIN_FILENO);
	std[1] = dup(STDOUT_FILENO);
	// dup2(cmd->fd[0], STDIN_FILENO);
	// if (cmd->fd[0] != STDIN_FILENO)
	// 	close(cmd->fd[0]);
	upd_stdfd(cmd, std);
	status = 0;
	while (cmd)
	{
		if (cmd->fd[0] != std[0])
		{
			dup2(cmd->fd[0], STDIN_FILENO);
			close(cmd->fd[0]);
		}
		if (cmd->next)
		{
			pipe(pp);
			dup2(pp[1], STDOUT_FILENO);
			close(pp[1]);
			if (cmd->fd[1] != std[1])
			{
				dup2(cmd->fd[1], STDOUT_FILENO);
				close(cmd->fd[1]);
			}
		}
		else
		{
			dup2(cmd->fd[1], STDOUT_FILENO);
			if (cmd->fd[1] != std[1])
				close(std[1]);
		}
		status = exec_cmd(cmd, envp);
		if (cmd->next != NULL)
		{
			dup2(pp[0], STDIN_FILENO);
			close(pp[0]);
		}
		cmd = cmd->next;
	}
	dup2(std[0], STDIN_FILENO);
	dup2(std[1], STDOUT_FILENO);
	close(std[0]);
	close(std[1]);
	return (status);
}

char	*pathsearch(char *cmd, char **paths)
{
	char *tmp[2];
	int i;

	i = -1;
	while (paths[++i])
	{
		tmp[0] = ft_strjoin(paths[i], "/");
		if (tmp[0] == NULL)
			return (NULL);
		tmp[1] = ft_strjoin(tmp[0], cmd);
		if (tmp[1] == NULL)
			return (NULL);
		free(tmp[0]);
		if (access(tmp[1], F_OK) == 0) // check for X_OK in execve
			return (tmp[1]);
		free(tmp[1]);
	}
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd(cmd, STDERR_FILENO);
	ft_putendl_fd(": command not found", STDERR_FILENO);
	// EXITSTATUS: 127
	return (NULL);
}

char	*cmdpath(char *cmd) // free cmd
{
	char **paths;
	char *env_path;
	char *cpath;

	if (ft_strchr(cmd, '/') && access(cmd, F_OK) == -1)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(cmd, STDERR_FILENO);
		ft_putendl_fd(": No such file or directory", STDERR_FILENO); // has a '\'
		free(cmd);
		// EXITSTATUS: 127
		return (NULL);
	}
	if (ft_strchr(cmd, '/') || isbuiltin(cmd)) // also check for PATH
		return (cmd);
	env_path = getenv("PATH"); // should we use our envp?
	if (!env_path)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(cmd, STDERR_FILENO);
		ft_putendl_fd(": command not found", STDERR_FILENO);
		// EXITSTATUS: 127
		return (0);
	}
	paths = ft_split(env_path, ':');
	cpath = pathsearch(cmd, paths);
	ft_splitfree(paths);
	free(cmd);
	return (cpath);
}

int	exec_simple(t_command *cmd, char **envp)
{
	int		std[2];
	int		status;

	status = 0;
	std[0] = dup(STDIN_FILENO);
	std[1] = dup(STDOUT_FILENO);
	dup2(cmd->fd[0], STDIN_FILENO);
	if (cmd->fd[0] != STDIN_FILENO)
		close(cmd->fd[0]);
	dup2(cmd->fd[1], STDOUT_FILENO);
	if (cmd->fd[1] != STDOUT_FILENO)
		close(cmd->fd[1]);
	status = exec_cmd(cmd, envp);
	dup2(std[0], STDIN_FILENO);
	dup2(std[1], STDOUT_FILENO);
	close(std[0]);
	close(std[1]);
	return (status);
}

int	executor(t_app *app)
{
	t_command	*cmd;

	cmd = app->commands_list;
	while (cmd)
	{
		cmd->str[0] = cmdpath(cmd->str[0]);
		if (!cmd->str[0])
			return (-1); // free stuff !
		cmd = cmd->next;
	}
	if (redirector(app->commands_list) == -1)
		return (-1);
	if (app->pipes_count > 0)
		return (exec_ppx(app->commands_list, app->envp));
	else
		return (exec_simple(app->commands_list, app->envp));
	return (0);
}

int loop(t_app *app)
{
	char	*t;

	app->input = readline(PROMPT);
	t = ft_strtrim(app->input, " ");
	free(app->input);
	app->input = t;
	if (!app->input)
	{
		ft_putendl_fd("exit", STDOUT_FILENO);
		exit(EXIT_SUCCESS);
	}
	if (app->input[0] == '\0')
		return (reset(app));
	add_history(app->input);
	if (!check_quotes(app->input))
		return (ft_error(2, app));
	if (!get_tokens(app))
		return (ft_error(1, app));
	parser(app);
	// printf("/////////////////////\n");
	// printf("input: %s\n", app->input);
	// printf("paths: %p\n", app->paths);
	// printf("pipes_count: %d\n", app->pipes_count);
	// printf("/////////////////////\n");
	// print_commands_list(app->commands_list);
	executor(app);
	// redirector(app->commands_list);
	// print_commands_fd(app->commands_list);
	// exit(0);
	// executor(app);
	// reset(app);
	// exit(1);
	return (1);
}

int	env_list_size(t_env_list *env_list)
{
	int			size;
	t_env_list	*tmp;

	size = 0;
	tmp = env_list;
	while (tmp)
	{
		size++;
		tmp = tmp->next;
	}
	return (size);
}

char	**getenvp(t_env_list *env_list)
{
	char		**envp;
	char		*tmp;
	int			i;
	t_env_list	*tmp_lst;

	envp = malloc(sizeof(char *) * (env_list_size(env_list) + 1));
	if (!envp)
		return (NULL);
	envp[env_list_size(env_list)] = NULL;
	tmp = NULL;
	tmp_lst = env_list;
	i = 0;
	while (tmp_lst)
	{
		tmp = ft_strjoin(tmp_lst->key, "="); // double check env_list formation
		envp[i] = ft_strjoin(tmp, tmp_lst->value);
		free(tmp);
		tmp_lst = tmp_lst->next;
		i++;
	}
	return (envp);
}

void	print_envp(char **envp)
{
	int	i;

	i = -1;
	while (envp[++i])
		printf("%s\n", envp[i]);
}

int	main(int argc, char **argv, char **envp)
{
	t_app	*app;
	int		status;

	(void)envp;
	if (argc != 1 || argv[1])
		exit(1);
	app = malloc(sizeof(t_app));
	if (!app)
		exit(1);
	ft_bzero(app, sizeof(t_app));
	// app->envp = envp_dup(envp);
	init_app(app);
	fill_env_list(app, envp);
	// print_envp(getenvp(app->env_list));
	// exit(1);
	status = loop(app);
	return (status);
}

// int	g_exit_status;

// char	*get_hostname(void)
// {
// 	char	*tmp;
// 	int		fd;
// 	char	*host;

// 	fd = open("/etc/hostname", 00);
// 	if (fd == -1)
// 		return (NULL);
// 	tmp = get_next_line(fd);
// 	if (tmp == NULL)
// 		return (NULL);
// 	close(fd);
// 	host = ft_substr(tmp, 0, ft_strchr(tmp, '.') - tmp);
// 	free(tmp);
// 	return (host);
// }

// char	*get_rlpwd(void)
// {
// 	char	*rlpwd;
// 	char	*home;
// 	char	*tmp;

// 	rlpwd = getcwd(NULL, 0);
// 	if (rlpwd == NULL)
// 		return (NULL);
// 	home = ft_strjoin(getenv("HOME"), "/");
// 	if (home == NULL)
// 		home = ft_strdup("/");
// 	if (ft_strncmp(rlpwd, home, ft_strlen(home)) == 0)
// 	{
// 		tmp = ft_substr(rlpwd, ft_strlen(home) - 1, ft_strlen(rlpwd));
// 		free(rlpwd);
// 		free(home);
// 		rlpwd = ft_strjoin("~", tmp);
// 		free(tmp);
// 		return (rlpwd);
// 	}
// 	return (NULL);
// }

// char	*set_prompt(void)
// {
// 	char	*host;
// 	char	*rlpwd;
// 	char	*ppt;
// 	char	*tmp;

// 	host = get_hostname();
// 	rlpwd = get_rlpwd();
// 	ppt = ft_strjoin(getenv("USER"), "@");
// 	tmp = ppt;
// 	ppt = ft_strjoin(ppt, host);
// 	free(tmp);
// 	tmp = ppt;
// 	ppt = ft_strjoin(ppt, ":");
// 	free(tmp);
// 	tmp = ppt;
// 	ppt = ft_strjoin(ppt, rlpwd);
// 	free(tmp);
// 	tmp = ppt;
// 	ppt = ft_strjoin(ppt, "$ ");
// 	free(tmp);
// 	free(host);
// 	free(rlpwd);
// 	return (ppt);
// }

/* int	main(int argc, char **argv, char **envp)
{
	(void)argc;
	(void)argv;
	(void)envp;
	char	*l;

	rl_clear_history();
	t_app	*app;

	app = malloc(sizeof(t_app));
	ft_bzero(app, sizeof(t_app));
	fill_env_list(app, envp);
	while (1)
	{
		l = readline(set_prompt());
		if (l == NULL) // ctrl-D (EOF)
		{
			// terminate(all) // function to free everything before exiting shell
			rl_clear_history();
			ft_putstr_fd("exit\n", STDOUT_FILENO);
			exit(g_exit_status);
		}
		add_history(l);


		// BUILTINS TEST
		if (ft_strncmp(l, "cd", 2) == 0)
		{
			ft_cd(app, ft_split(l, ' '));
		}
		if (ft_strncmp(l, "echo", 4) == 0)
		{
			ft_echo(app, ft_split(l, ' '));
		}
		if (ft_strncmp(l, "exit", 4) == 0)
		{
			ft_exit(app, ft_split(l, ' '));
		}
		if (ft_strncmp(l, "export", 6) == 0)
		{
			ft_export(app, ft_split(l, ' '));
		}
		if (ft_strncmp(l, "pwd", 3) == 0)
		{
			ft_pwd(app, ft_split(l, ' '));
		}
		if (ft_strncmp(l, "unset", 5) == 0)
		{
			ft_unset(app, ft_split(l, ' '));
		}
		if (ft_strncmp(l, "env", 3) == 0)
		{
			ft_env(app, ft_split(l, ' '));
		}
		// TEST END

		free(l);
	}
	return (EXIT_SUCCESS);
} */
