/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rerodrig <rerodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/04 19:32:46 by amitcul           #+#    #+#             */
/*   Updated: 2024/07/12 14:58:03 by rerodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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
void print_commands_list(t_command *commands_list)
{
    t_command *tmp;
    t_lexer_token *token;

    tmp = commands_list;
    printf("\n");

    while (tmp)
    {
        // Validate that tmp and its fields are not NULL
        if (tmp == NULL)
        {
            printf("Error: tmp is NULL\n");
            return;
        }

        // Ensure tmp->str is properly allocated
        if (tmp->str)
        {
            printf("command: %s\n", tmp->str[0] ? tmp->str[0] : "(null)");
            printf("args:\n");
            for (int i = 1; tmp->str[i]; i++)
                printf("_%s\n", tmp->str[i] ? tmp->str[i] : "(null)");
        }
        else
        {
            printf("command: (null)\n");
            printf("args: (none)\n");
        }

        // Check if tmp->heredoc is properly initialized
        if (tmp->heredoc)
        {
            printf("heredoc: %s\n", tmp->heredoc);
        }
        else
        {
            printf("heredoc: (null)\n");
        }

        printf("redirs___\n");
        token = tmp->redirs;
        while (token)
        {
            // Ensure token is not NULL and token->str is not null
            if (token == NULL)
            {
                printf("Error: token is NULL\n");
                return;
            }
            printf("_str: %s\n", token->str ? token->str : "(null)");
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
	// t_command	*tmp;
	
	// tmp = cmd;
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

// int loop(t_app *app)
// {
// 	char	*t;

// 	app->input = readline(PROMPT);
// 	t = ft_strtrim(app->input, " ");
// 	free(app->input);
// 	app->input = t;
// 	if (!app->input)
// 	{
// 		ft_putendl_fd("exit", STDOUT_FILENO);
// 		exit(EXIT_SUCCESS);
// 	}
// 	if (app->input[0] == '\0')
// 		return (reset(app));
// 	add_history(app->input);
// 	if (!check_quotes(app->input))
// 		return (ft_error(2, app));
// 	// if (!get_tokens(app))
// 	// 	return (ft_error(1, app));
// 	///////////////////////////////////////////////////////////////////////
// 		int	i;
// 		int	j;

// 		i = 0;
// 		while (app->input[i])
// 		{
// 			j = 0;
// 			while (ft_iswhitespace(app->input[i]))
// 				i++;
			
// 			// Check if the current character is a token
// 			t_token_type token_type;
// 			if (app->input[i] == '|')
// 				token_type = PIPE;
// 			else if (app->input[i] == '>')
// 				token_type = GREAT;
// 			else if (app->input[i] == '<')
// 				token_type = LESS;
// 			else
// 				token_type = 0;

// 			// Handle token
// 			if (token_type)
// 			{
// 				if (token_type == GREAT && app->input[i + 1] == '>')
// 				{
// 					if (!add_node(NULL, G_GREAT, &app->lexer_tokens))
// 						return (ft_error(1, app));
// 					j = 2;
// 				}
// 				else if (token_type == LESS && app->input[i + 1] == '<')
// 				{
// 					if (!add_node(NULL, L_LESS, &app->lexer_tokens))
// 						return (ft_error(1, app));
// 					j = 2;
// 				}
// 				else
// 				{
// 					if (!add_node(NULL, token_type, &app->lexer_tokens))
// 						return (ft_error(1, app));
// 					j = 1;
// 				}
// 			}
// 			else // Read word
// 			{
// 				int k = 0;
// 				while (app->input[i + k] && !(app->input[i + k] == '|' || app->input[i + k] == '>' || app->input[i + k] == '<'))
// 				{
// 					k += handle_quotes(i + k, app->input, '"');
// 					k += handle_quotes(i + k, app->input, '\'');
// 					if (ft_iswhitespace(app->input[i + k]))
// 						break ;
// 					k++;
// 				}
// 				if (!add_node(ft_substr(app->input, i, k), 0, &app->lexer_tokens))
// 					return (ft_error(1, app));
// 				j = k;
// 			}

// 			i += j;
// 		}

// 	parser(app);
// 	// printf("/////////////////////\n");
// 	// printf("input: %s\n", app->input);
// 	// printf("paths: %p\n", app->paths);
// 	// printf("pipes_count: %d\n", app->pipes_count);
// 	// printf("/////////////////////\n");
// 	// print_commands_list(app->commands_list);
// 	executor(app);
// 	// redirector(app->commands_list);
// 	// print_commands_fd(app->commands_list);
// 	// exit(0);
// 	// executor(app);
// 	// reset(app);
// 	// exit(1);
// 	return (1);
// }
// static t_command	*init_command(char **str, size_t redirs_count,
// 		t_lexer_token *redirs)
// {
// 	t_command	*command;

// 	command = (t_command *)malloc(sizeof(t_command));
// 	if (!command)
// 		return (NULL);
// 	command->str = str;
// 	command->redirs_count = redirs_count;
// 	command->redirs = redirs;
// 	command->heredoc = NULL; // Initialize heredoc to NULL
// 	command->next = NULL;
// 	command->prev = NULL;
// 	return (command);
// }
// static size_t	count_args(t_lexer_token *list)
// {
// 	size_t	count;

// 	count = 0;
// 	while (list && list->token_type != PIPE)
// 	{
// 		count += 1;
// 		list = list->next;
// 	}
// 	return (count);
// }

// static t_command	*get_command(t_parser *parser)
// {
// 	char			**str;
// 	size_t			i;
// 	size_t			args_count;
// 	t_lexer_token	*curr;

// 	collect_redirections(parser);
// 	args_count = count_args(parser->lexer_list);
// 	str = (char **) ft_calloc(args_count + 1, sizeof(char *));
// 	if (!str)
// 		exit(2); //! Handle
// 	curr = parser->lexer_list;
// 	i = 0;
// 	while (args_count > 0)
// 	{
// 		if (curr->str)
// 		{
// 			str[i] = ft_strdup(curr->str);
// 			delete_node_by_index(&parser->lexer_list, curr->index);
// 			curr = parser->lexer_list;
// 			i++;
// 		}
// 		args_count -= 1;
// 	}
// 	return (init_command(str, parser->redirs_count, parser->redirs));
// }

int loop(t_app *app)
{
    char *t;

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

    ///////////////////////////////////////////////////////////////////////
    int i;
    int j;

    i = 0;
    while (app->input[i])
    {
        j = 0;
        while (ft_iswhitespace(app->input[i]))
            i++;

        // Check if the current character is a token
        t_token_type token_type;
        if (app->input[i] == '|')
            token_type = PIPE;
        else if (app->input[i] == '>')
            token_type = GREAT;
        else if (app->input[i] == '<')
            token_type = LESS;
        else
            token_type = 0;

        // Handle token
        if (token_type)
        {
            if (token_type == GREAT && app->input[i + 1] == '>')
            {
                if (!add_node(NULL, G_GREAT, &app->lexer_tokens))
                    return (ft_error(1, app));
                j = 2;
            }
            else if (token_type == LESS && app->input[i + 1] == '<')
            {
                if (!add_node(NULL, L_LESS, &app->lexer_tokens))
                    return (ft_error(1, app));
                j = 2;
            }
            else
            {
                if (!add_node(NULL, token_type, &app->lexer_tokens))
                    return (ft_error(1, app));
                j = 1;
            }
        }
        else // Read word
        {
            int k = 0;
            while (app->input[i + k] && !(app->input[i + k] == '|' || app->input[i + k] == '>' || app->input[i + k] == '<'))
            {
                k += handle_quotes(i + k, app->input, '"');
                k += handle_quotes(i + k, app->input, '\'');
                if (ft_iswhitespace(app->input[i + k]))
                    break;
                k++;
            }
            if (!add_node(ft_substr(app->input, i, k), 0, &app->lexer_tokens))
                return (ft_error(1, app));
            j = k;
        }

        i += j;
    }

    ///////////////////////////////////////////////////////////////////////
    // Inlined parser function
    t_parser *parser;
    t_command *command;

    parser = NULL;
    if (app->lexer_tokens->token_type == PIPE)
    {
        // Inline parser_double_token_error
        ft_putstr_fd("minishell: syntax error near unexpected token ", STDERR_FILENO);
        ft_putstr_fd("'|'\n", STDERR_FILENO);
        ft_lexerclear(&app->lexer_tokens);
        reset(app);
        return (EXIT_FAILURE);
    }

    // Inline count_pipes
    t_lexer_token *tmp = app->lexer_tokens;
    app->pipes_count = 0;
    while (tmp)
    {
        if (tmp->token_type == PIPE)
            app->pipes_count += 1;
        tmp = tmp->next;
    }

    while (app->lexer_tokens)
    {
        if (app->lexer_tokens->token_type == PIPE)
        {
            // Inline delete_node_by_index
            t_lexer_token *curr = app->lexer_tokens;
            while (curr != NULL)
            {
                if (curr->index == app->lexer_tokens->index)
                {
                    if (curr->prev != NULL)
                        curr->prev->next = curr->next;
                    else
                        app->lexer_tokens = curr->next;
                    if (curr->next != NULL)
                        curr->next->prev = curr->prev;
                }
                curr = curr->next;
            }
        }

        // Inline handle_pipe_errors
        if (app->lexer_tokens->token_type == PIPE)
        {
            ft_putstr_fd("minishell: syntax error near unexpected token ", STDERR_FILENO);
            ft_putstr_fd("'|'\n", STDERR_FILENO);
            ft_lexerclear(&app->lexer_tokens);
            reset(app);
            return (EXIT_FAILURE);
        }
        if (!app->lexer_tokens)
        {
            ft_lexerclear(&app->lexer_tokens);
            ft_error(0, app);
            return (EXIT_FAILURE);
        }

        // Inline init_parser
        parser = (t_parser *)malloc(sizeof(t_parser));
        if (!parser)
        {
            // Handle malloc failure
            ft_error(0, app);
            return (EXIT_FAILURE);
        }
        parser->app = app;
        parser->lexer_list = app->lexer_tokens;
        parser->redirs = NULL;
        parser->redirs_count = 0;

        ///////////////////////////////////////////////////////////////////////
        // Inline collect_redirections
        t_lexer_token *curr = parser->lexer_list;
        while (curr && curr->token_type == WORD)
            curr = curr->next;
        if (curr == NULL || curr->token_type == PIPE)
            ; // No redirections to process
        else
        {
            if (curr->next == NULL)
            {
                // Handle error if the next token is missing
                ft_putstr_fd("minishell: syntax error near unexpected token\n", STDERR_FILENO);
                ft_lexerclear(&parser->lexer_list);
                ft_error(0, app);
                free(parser);
                return (EXIT_FAILURE);
            }
            if (curr->next->token_type > WORD)
            {
                // Handle unexpected token after redirection operator
                ft_putstr_fd("minishell: syntax error near unexpected token\n", STDERR_FILENO);
                ft_lexerclear(&parser->lexer_list);
                ft_error(0, app);
                free(parser);
                return (EXIT_FAILURE);
            }
            if (curr->token_type >= GREAT && curr->token_type <= L_LESS)
            {
                // Inline add_new_redirect
                t_lexer_token *new;
                size_t first;
                size_t second;

                new = get_lexer_new_node(ft_strdup(curr->next->str), curr->token_type);
                if (!new)
                {
                    // Handle error when allocating new node
                    ft_lexerclear(&parser->lexer_list);
                    ft_error(1, app);
                    free(parser);
                    return (EXIT_FAILURE);
                }

                // Inline push_back
                t_lexer_token *currentt;

                if (parser->redirs == NULL)
                {
                    new->prev = NULL;
                    parser->redirs = new;
                }
                else
                {
                    currentt = parser->redirs;
                    while (currentt->next != NULL)
                        currentt = currentt->next;
                    currentt->next = new;
                    new->prev = currentt;
                }

                first = curr->index;
                second = curr->next->index;

                // Inline delete_node_by_index
                t_lexer_token *tmp_curr = parser->lexer_list;
                while (tmp_curr != NULL)
                {
                    if (tmp_curr->index == first || tmp_curr->index == second)
                    {
                        if (tmp_curr->prev != NULL)
                            tmp_curr->prev->next = tmp_curr->next;
                        else
                            parser->lexer_list = tmp_curr->next;
                        if (tmp_curr->next != NULL)
                            tmp_curr->next->prev = tmp_curr->prev;
                    }
                    tmp_curr = tmp_curr->next;
                }

                parser->redirs_count += 1;
            }
            // Recursive call to handle additional redirections
            curr = parser->lexer_list;
        }

        ///////////////////////////////////////////////////////////////////////
        // Inline count_args
        size_t count = 0;
        t_lexer_token *list = parser->lexer_list;
        while (list && list->token_type != PIPE)
        {
            count += 1;
            list = list->next;
        }
        size_t args_count = count;

        char **str = (char **)ft_calloc(args_count + 1, sizeof(char *));
        if (!str)
        {
            ft_error(2, app);
            free(parser);
            return (EXIT_FAILURE);
        }
        curr = parser->lexer_list;
        size_t i = 0;
        while (args_count > 0)
        {
            if (curr->str)
            {
                str[i] = ft_strdup(curr->str);
                // Inline delete_node_by_index
                t_lexer_token *tmp_curr = curr;
                while (tmp_curr != NULL)
                {
                    if (tmp_curr->index == curr->index)
                    {
                        if (tmp_curr->prev != NULL)
                            tmp_curr->prev->next = tmp_curr->next;
                        else
                            parser->lexer_list = tmp_curr->next;
                        if (tmp_curr->next != NULL)
                            tmp_curr->next->prev = tmp_curr->prev;
                    }
                    tmp_curr = tmp_curr->next;
                }
                curr = parser->lexer_list;
                i++;
            }
            args_count -= 1;
        }

        ///////////////////////////////////////////////////////////////////////
        // Inline init_command
        command = (t_command *)malloc(sizeof(t_command));
        if (!command)
        {
            // Inline parser_error
            ft_lexerclear(&parser->lexer_list);
            ft_error(0, app);
            free(parser);
            return (EXIT_FAILURE);
        }
        command->str = str;
        command->redirs_count = parser->redirs_count;
        command->redirs = parser->redirs;
        command->heredoc = NULL; // Initialize heredoc to NULL
        command->next = NULL;
        command->prev = NULL;

        if (!app->commands_list)
        {
            app->commands_list = command;
        }
        else
        {
            // Inline add_command_to_list
            t_command *curr_command = app->commands_list;
            while (curr_command->next != NULL)
                curr_command = curr_command->next;
            curr_command->next = command;
            command->prev = curr_command;
        }

        app->lexer_tokens = parser->lexer_list;

        // Debug information
        printf("/////////////////////\n");
        printf("input: %s\n", app->input);
        printf("paths: %p\n", app->paths);
        printf("pipes_count: %d\n", app->pipes_count);
        printf("/////////////////////\n");
        print_commands_list(app->commands_list);
        executor(app);

        free(parser); // Free parser structure after use to avoid memory leaks
    }
    ///////////////////////////////////////////////////////////////////////

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
