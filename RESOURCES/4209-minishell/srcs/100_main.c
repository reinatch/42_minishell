/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   100_main.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoda-s <antoda-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/17 19:27:05 by antoda-s          #+#    #+#             */
/*   Updated: 2024/04/03 23:55:49 by antoda-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

int	g_exit_status;

/// @brief 		Updates the shell level variable (MSHLVL)
/// @param s 	Struct with pparams to initialize
void	update_shlvl(t_script *s)
{
	char	*shlvl;
	char	*new_shlvl;
	int		i;

	shlvl = env_var_getter("MSHLVL", s->envp, NULL);
	if (!shlvl)
	{
		env_var_setter("1", "MSHLVL", &s->envp);
		return ;
	}
	i = ft_atoi(shlvl);
	new_shlvl = ft_itoa(i + 1);
	if (!new_shlvl)
	{
		error_return("", errno, 1);
		return ;
	}
	env_var_setter(new_shlvl, "MSHLVL", &s->envp);
	ft_free(shlvl);
	ft_free(new_shlvl);
}

/// @brief 		Creates array from system environment variables
/// @param envp system environment variables from main (... char **envp)
/// @return 	array copy of system environment variables
char	**envp_init(char **envp)
{
	char	**ms_envp;
	int		i;

	i = 0;
	while (envp[i])
		i++;
	ms_envp = ft_calloc(i + 1, sizeof(char *));
	if (!ms_envp)
	{
		error_return("", errno, 1);
		return (NULL);
	}
	i = -1;
	while (envp[++i])
	{
		ms_envp[i] = ft_strdup(envp[i]);
		if (!ms_envp[i])
		{
			error_return("", errno, 1);
			return (NULL);
		}
	}
	ms_envp[i] = NULL;
	return (ms_envp);
}

/// @brief 		Initialize basic int
/// @param s 	struct with pparams to initialize
void	struct_init(t_script *s)
{
	s->path = NULL;
	s->hd = NULL;
	s->cmd_count = 0;
	s->exit_status = 0;
}

/// @brief 				Initializes the shell and keeps looping until exit
/// @param script		Script structure (see minishell struct)
/// @return				void
// int	ms_loop(t_script *s)
// {
// 	int		status;
// 	char	*line_buffer;

// 	line_buffer = NULL;
// 	while (1)
// 	{
// 		struct_init(s);
// 		signal_setter();
// 		status = parser(s, &line_buffer);
// 		ft_free(line_buffer);
// 		if (status == 1)
// 			continue ;
// 		else if (status == 2)
// 		{
// 			ft_putendl_fd("exit", 2);
// 			break ;
// 		}
// 		if (s->cmd_count > 0)
// 			if (execute(s))
// 				break ;
// 		free_commands(s->cmds, s->cmd_count);
// 	}
// 	if (s->cmd_count > 0)
// 		free_commands(s->cmds, s->cmd_count);
// 	return (0);
// }
void print_command(const t_command *cmd)
{
    if (cmd == NULL) {
        printf("Command: NULL\n");
        return;
    }

    printf("Command:\n");
    printf("  cmd: %s\n", cmd->cmd ? cmd->cmd : "NULL");
    printf("  argc: %d\n", cmd->argc);

    printf("  argv:\n");
    if (cmd->argv == NULL) {
        printf("    NULL\n");
    } else {
        for (int i = 0; cmd->argv[i] != NULL; i++) {
            printf("    argv[%d]: %s\n", i, cmd->argv[i]);
        }
    }

    printf("  Redirection In:\n");
    printf("    name: %s\n", cmd->in.name ? cmd->in.name : "NULL");
    printf("    flag: %d\n", cmd->in.flag);
    // printf("    heredoc: %s\n", cmd->in.heredoc ? cmd->in.heredoc : "NULL");

    printf("  Redirection Out:\n");
    printf("    name: %s\n", cmd->out.name ? cmd->out.name : "NULL");
    printf("    flag: %d\n", cmd->out.flag);
}

void print_commands(const t_command *cmds, int count)
{
    if (cmds == NULL) {
        printf("Commands: NULL\n");
        return;
    }

    for (int i = 0; i < count; i++) {
        printf("Command %d:\n", i);
        print_command(&cmds[i]);
        printf("\n");
    }
}

int ms_loop(t_script *s)
{
    int status;
    char *line_buffer;
    t_token *tk;
    t_token *tk_ptr;
    char *content;
    int cmd_count;
    int i;
    t_token *tmp;

    line_buffer = NULL;
    while (1)
    {
        struct_init(s);
        signal_setter();
        
        // Read line input
        line_buffer = readline("\001\033[1;94m\002minishell > \001\033[0m\002");
        if (!line_buffer)
        {
            fprintf(stderr, "Error: readline failed\n");
            line_buffer = ft_strdup("exit");
        }
        add_history(line_buffer);
        
        // Initialize token list
        tk = NULL;
        char *str = line_buffer;
        t_ops ptr;
        char *prev = str;

        while (str && *str)
        {
            static const t_ops ops[16] = {
                {"<<", 2, TK_I}, {"<", 1, TK_I}, {">>", 2, TK_O},
                {">|", 2, TK_O}, {"<>", 2, TK_O}, {">", 1, TK_O},
                {" ", 1, TK_WS}, {"\n", 1, TK_WS}, {"\v", 1, TK_WS},
                {"\t", 1, TK_WS}, {"\r", 1, TK_WS}, {"\f", 1, TK_WS},
                {"||", 2, TK_OR}, {"&&", 2, TK_AND}, {"|", 1, TK_PIPE},
                {NULL, 1, 0}
            };
            ptr = (t_ops){0, 0, 0};
            int i = -1;
            while (ops[++i].op)
            {
                if (!ft_strncmp(str, ops[i].op, ops[i].size))
                {
                    ptr = ops[i];
                    break;
                }
            }

            if (ptr.op != 0 && prev != str)
                tk_lst_addback(&tk, tk_addnew(prev, str - prev, TK_NAME));

            if (ptr.op != 0)
            {
                str += ptr.size;
                if (ptr.type != TK_WS)
                    tk_lst_addback(&tk, tk_addnew(ptr.op, ptr.size, ptr.type));
                prev = str;
            }
            else if ((*str == '\"' || *str == '\'') && tk_quotes_checker(&str))
            {
                status = ERROR;
                break;
            }
            else
                ++str;
        }

        if (prev != str)
            tk_lst_addback(&tk, tk_addnew(prev, str - prev, TK_NAME));

        if (status == ERROR) {
            error_return("Syntax Error", 1, 0);
            ft_free(line_buffer);
            continue;
        }
        
        // Token expansion
        tk_ptr = tk;
        while (tk_ptr) {
            content = tk_ptr->content;
            if (content[0] == '~' && ft_strlen(content) == 1 && s->home)
                tk_ptr->content = ft_strdup(s->home);
            else
                tk_ptr->content = tk_env_var_expander(content, s);
            ft_free(content);
            tk_ptr = tk_ptr->next;
        }
        status = SUCCESS;

        // Inline tk_rm_blank
        if (tk) {
            t_token **tk_p = &tk;
            t_token *tk_n = tk;
            char c;
            while (tk_n) {
                if (tk_n->type == TK_NAME && tk_n->rm) {
                    c = ft_strset(tk_n->content, " \t\v\r\n\f");
                    if (c)
                        tk_blank_split(tk_p, tk_n, c);
                }
                tk_p = &(tk_n->next);
                tk_n = tk_n->next;
            }
        }

        // Inline syntax_checker
        if (tk && tk->type == TK_PIPE) {
            status = syntax_error_msg(tk->content, 2, 0);
            free_tokens(&tk);
            ft_free(line_buffer);
            continue;
        }
        tk_ptr = tk;
        while (tk_ptr) {
            if (!tk_ptr->next && (tk_ptr->type == TK_PIPE || tk_ptr->type == TK_OR)) {
                status = syntax_error_msg(tk_ptr->content, 2, 0);
                free_tokens(&tk);
                ft_free(line_buffer);
                continue;
            }
            if (!tk_ptr->next && (tk_ptr->type == TK_I || tk_ptr->type == TK_O)) {
                status = syntax_error_msg("newline", 2, 0);
                free_tokens(&tk);
                ft_free(line_buffer);
                continue;
            }
            if (tk_ptr->next && ((tk_ptr->type == TK_PIPE && tk_ptr->next->type == TK_PIPE)
                    || (tk_ptr->type == TK_PIPE && tk_ptr->next->type == TK_OR)
                    || (tk_ptr->type == TK_OR && tk_ptr->next->type == TK_OR)
                    || (tk_ptr->type == TK_OR && tk_ptr->next->type == TK_PIPE))) {
                status = syntax_error_msg(tk_ptr->next->content, 2, 0);
                free_tokens(&tk);
                ft_free(line_buffer);
                continue;
            }
            if ((tk_ptr->type == TK_O || tk_ptr->type == TK_I)
                && (tk_ptr->next && tk_ptr->next->type != TK_NAME)) {
                status = syntax_error_msg(tk_ptr->next->content, 2, 0);
                free_tokens(&tk);
                ft_free(line_buffer);
                continue;
            }
            if ((tk_ptr->type == TK_O || tk_ptr->type == TK_I)
                && (tk_ptr->next && (!ft_strncmp(tk_ptr->next->content, "&", 1)))) {
                status = syntax_error_msg("newline", 2, 0);
                free_tokens(&tk);
                ft_free(line_buffer);
                continue;
            }
            tk_ptr = tk_ptr->next;
        }
        
        // Inline cmds_counter
        cmd_count = 0;
        tk_ptr = tk;
        while (tk_ptr) {
            if (!cmd_count)
                cmd_count = 1;
            if (tk_ptr->type == TK_PIPE)
                cmd_count++;
            tk_ptr = tk_ptr->next;
        }
        s->cmd_count = cmd_count;

        s->cmds = ft_calloc(s->cmd_count, sizeof(t_command));
        if (!s->cmds) {
            status = error_return("", errno, 1);
            ft_free(line_buffer);
            continue;
        }
        
        // Inline fnames_clear
        for (i = 0; i < s->cmd_count; ++i) {
            s->cmds[i].in.name = NULL;
            s->cmds[i].out.name = NULL;
            s->cmds[i].in.heredoc = NULL;
            s->cmds[i].in.flag = -1;
            s->cmds[i].out.flag = -1;
        }
        
        // Inline args_counter
        i = 0;
        while (i < s->cmd_count) {
            s->cmds[i].argc = 0;
            tmp = tk;
            while (tk && tk->type != TK_PIPE) {
                if (tk->type == TK_NAME && (tmp->type != TK_I && tmp->type != TK_O))
                    s->cmds[i].argc++;
                tmp = tk;
                tk = tk->next;
            }
            if (tk)
                tk = tk->next;
            i++;
        }

        if (s->cmd_count == 0) {
            // Inline back_to_loop
            free_commands(s->cmds, s->cmd_count);
            free_tokens(&tk);
            ft_free(line_buffer);
            continue;
        }
        
        // Inline parse_commands
        i = 0;
        while (tk) {
            s->cmds[i].argv = ft_calloc((s->cmds[i].argc + 1), sizeof(char *));
            if (!s->cmds[i].argv)
            {
                status = error_return("", errno, 1);
                free_commands(s->cmds, s->cmd_count);
                free_tokens(&tk);
                ft_free(line_buffer);
                continue;
            }
            int j = 0;
            while (tk && tk->type != TK_PIPE)
            {
                if (tk->type == TK_NAME)
                    s->cmds[i].argv[j++] = ft_strdup(tk->content);
                else if (tk->type == TK_I && redir(tk, &s->cmds[i].in))
                {
                    free_commands(s->cmds, s->cmd_count);
                    free_tokens(&tk);
                    ft_free(line_buffer);
                    status = 1;
                    break;
                }
                else if (tk->type == TK_O && redir(tk, &s->cmds[i].out))
                {
                    free_commands(s->cmds, s->cmd_count);
                    free_tokens(&tk);
                    ft_free(line_buffer);
                    status = 1;
                    break;
                }
                if (tk->type == TK_I || tk->type == TK_O)
                    tk = tk->next;
                if (tk)
                    tk = tk->next;
            }
            if (tk)
                tk = tk->next;
            s->cmds[i].argv[j] = NULL;
            i++;
        }

        if (parse_commands(tk, s->cmds, 0, 0))
        {
            free_tokens(&tk);
            ft_free(line_buffer);
            continue;
        }
        free_tokens(&tk);
        status = SUCCESS;

        ft_free(line_buffer);
        if (status == 1)
            continue;
        else if (status == 2)
        {
            ft_putendl_fd("exit", 2);
            break;
        }
        print_commands(s->cmds, s->cmd_count);
        // if (s->cmd_count > 0)
        //     if (execute(s))
        //         break;
        // free_commands(s->cmds, s->cmd_count);
    }
    if (s->cmd_count > 0)
        free_commands(s->cmds, s->cmd_count);
    return (0);
}

/// @brief 			Main function MINISHELL
/// @param argc		Number of arguments (NOT USED)
/// @param argv		Arguments (NOT USED)
/// @param envp		Environment variables
/// @return
int	main(int argc, char **argv, char **envp)
{
	t_script	s;

	(void)argc;
	(void)argv;
	s.envp = envp_init(envp);
	update_shlvl(&s);
	s.envt = ft_calloc(1, sizeof(char *));
	if (!s.envt)
	{
		error_return("", errno, 1);
		return (1);
	}
	s.envt[0] = NULL;
	termios_getter(&s.termios_p);
	s.home = env_var_getter("HOME", s.envp, NULL);
	g_exit_status = bi_help();
	ms_loop(&s);
	return (free_array(s.envp, 0));
}
