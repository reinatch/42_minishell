

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
int	ms_loop(t_script *s)
{
	int		status;
	char	*line_buffer;

	line_buffer = NULL;
	while (1)
	{
		struct_init(s);
		signal_setter();
		status = parser(s, &line_buffer);
		ft_free(line_buffer);
		if (status == 1)
			continue ;
		else if (status == 2)
		{
			ft_putendl_fd("exit", 2);
			break ;
		}
		if (s->cmd_count > 0)
			if (execute(s))
				break ;
		free_commands(s->cmds, s->cmd_count);
	}
	if (s->cmd_count > 0)
		free_commands(s->cmds, s->cmd_count);
	return (0);
}
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

// int ms_loop(t_script *s)
// {
//     int status;
//     char *line_buffer;
//     t_token *tk;
//     t_token *tk_ptr;
//     char *content;
//     int cmd_count;
//     int i;
//     t_token *tmp;

//     line_buffer = NULL;
//     while (1)
//     {
//         struct_init(s);
//         signal_setter();
        
//         // Read line input
//         line_buffer = readline("\001\033[1;94m\002minishell > \001\033[0m\002");
//         if (!line_buffer)
//         {
//             fprintf(stderr, "Error: readline failed\n");
//             line_buffer = ft_strdup("exit");
//         }
//         add_history(line_buffer);
        
//         // Initialize token list
//         tk = NULL;
//         char *str = line_buffer;
//         t_ops ptr;
//         char *prev = str;

//         while (str && *str)
//         {
//             static const t_ops ops[16] = {
//                 {"<<", 2, TK_I}, {"<", 1, TK_I}, {">>", 2, TK_O},
//                 {">|", 2, TK_O}, {"<>", 2, TK_O}, {">", 1, TK_O},
//                 {" ", 1, TK_WS}, {"\n", 1, TK_WS}, {"\v", 1, TK_WS},
//                 {"\t", 1, TK_WS}, {"\r", 1, TK_WS}, {"\f", 1, TK_WS},
//                 {"||", 2, TK_OR}, {"&&", 2, TK_AND}, {"|", 1, TK_PIPE},
//                 {NULL, 1, 0}
//             };
//             ptr = (t_ops){0, 0, 0};
//             int i = -1;
//             while (ops[++i].op)
//             {
//                 if (!ft_strncmp(str, ops[i].op, ops[i].size))
//                 {
//                     ptr = ops[i];
//                     break;
//                 }
//             }

//             if (ptr.op != 0 && prev != str)
//                 tk_lst_addback(&tk, tk_addnew(prev, str - prev, TK_NAME));

//             if (ptr.op != 0)
//             {
//                 str += ptr.size;
//                 if (ptr.type != TK_WS)
//                     tk_lst_addback(&tk, tk_addnew(ptr.op, ptr.size, ptr.type));
//                 prev = str;
//             }
//             else if ((*str == '\"' || *str == '\'') && tk_quotes_checker(&str))
//             {
//                 status = ERROR;
//                 break;
//             }
//             else
//                 ++str;
//         }

//         if (prev != str)
//             tk_lst_addback(&tk, tk_addnew(prev, str - prev, TK_NAME));

//         if (status == ERROR) {
//             error_return("Syntax Error", 1, 0);
//             ft_free(line_buffer);
//             continue;
//         }
        
//         // Token expansion
//         tk_ptr = tk;
//         while (tk_ptr) {
//             content = tk_ptr->content;
//             if (content[0] == '~' && ft_strlen(content) == 1 && s->home)
//                 tk_ptr->content = ft_strdup(s->home);
//             else
//                 tk_ptr->content = tk_env_var_expander(content, s);
//             ft_free(content);
//             tk_ptr = tk_ptr->next;
//         }
//         status = SUCCESS;

//         // Inline tk_rm_blank
//         if (tk) {
//             t_token **tk_p = &tk;
//             t_token *tk_n = tk;
//             char c;
//             while (tk_n) {
//                 if (tk_n->type == TK_NAME && tk_n->rm) {
//                     c = ft_strset(tk_n->content, " \t\v\r\n\f");
//                     if (c)
//                         tk_blank_split(tk_p, tk_n, c);
//                 }
//                 tk_p = &(tk_n->next);
//                 tk_n = tk_n->next;
//             }
//         }

//         // Inline syntax_checker
//         if (tk && tk->type == TK_PIPE) {
//             status = syntax_error_msg(tk->content, 2, 0);
//             free_tokens(&tk);
//             ft_free(line_buffer);
//             continue;
//         }
//         tk_ptr = tk;
//         while (tk_ptr) {
//             if (!tk_ptr->next && (tk_ptr->type == TK_PIPE || tk_ptr->type == TK_OR)) {
//                 status = syntax_error_msg(tk_ptr->content, 2, 0);
//                 free_tokens(&tk);
//                 ft_free(line_buffer);
//                 continue;
//             }
//             if (!tk_ptr->next && (tk_ptr->type == TK_I || tk_ptr->type == TK_O)) {
//                 status = syntax_error_msg("newline", 2, 0);
//                 free_tokens(&tk);
//                 ft_free(line_buffer);
//                 continue;
//             }
//             if (tk_ptr->next && ((tk_ptr->type == TK_PIPE && tk_ptr->next->type == TK_PIPE)
//                     || (tk_ptr->type == TK_PIPE && tk_ptr->next->type == TK_OR)
//                     || (tk_ptr->type == TK_OR && tk_ptr->next->type == TK_OR)
//                     || (tk_ptr->type == TK_OR && tk_ptr->next->type == TK_PIPE))) {
//                 status = syntax_error_msg(tk_ptr->next->content, 2, 0);
//                 free_tokens(&tk);
//                 ft_free(line_buffer);
//                 continue;
//             }
//             if ((tk_ptr->type == TK_O || tk_ptr->type == TK_I)
//                 && (tk_ptr->next && tk_ptr->next->type != TK_NAME)) {
//                 status = syntax_error_msg(tk_ptr->next->content, 2, 0);
//                 free_tokens(&tk);
//                 ft_free(line_buffer);
//                 continue;
//             }
//             if ((tk_ptr->type == TK_O || tk_ptr->type == TK_I)
//                 && (tk_ptr->next && (!ft_strncmp(tk_ptr->next->content, "&", 1)))) {
//                 status = syntax_error_msg("newline", 2, 0);
//                 free_tokens(&tk);
//                 ft_free(line_buffer);
//                 continue;
//             }
//             tk_ptr = tk_ptr->next;
//         }
        
//         // Inline cmds_counter
//         cmd_count = 0;
//         tk_ptr = tk;
//         while (tk_ptr) {
//             if (!cmd_count)
//                 cmd_count = 1;
//             if (tk_ptr->type == TK_PIPE)
//                 cmd_count++;
//             tk_ptr = tk_ptr->next;
//         }
//         s->cmd_count = cmd_count;

//         s->cmds = ft_calloc(s->cmd_count, sizeof(t_command));
//         if (!s->cmds) {
//             status = error_return("", errno, 1);
//             ft_free(line_buffer);
//             continue;
//         }
        
//         // Inline fnames_clear
//         for (i = 0; i < s->cmd_count; ++i) {
//             s->cmds[i].in.name = NULL;
//             s->cmds[i].out.name = NULL;
//             s->cmds[i].in.heredoc = NULL;
//             s->cmds[i].in.flag = -1;
//             s->cmds[i].out.flag = -1;
//         }
        
//         // Inline args_counter
//         i = 0;
//         while (i < s->cmd_count) {
//             s->cmds[i].argc = 0;
//             tmp = tk;
//             while (tk && tk->type != TK_PIPE) {
//                 if (tk->type == TK_NAME && (tmp->type != TK_I && tmp->type != TK_O))
//                     s->cmds[i].argc++;
//                 tmp = tk;
//                 tk = tk->next;
//             }
//             if (tk)
//                 tk = tk->next;
//             i++;
//         }

//         if (s->cmd_count == 0) {
//             // Inline back_to_loop
//             free_commands(s->cmds, s->cmd_count);
//             free_tokens(&tk);
//             ft_free(line_buffer);
//             continue;
//         }
        
//         // Inline parse_commands
//         i = 0;
//         while (tk) {
//             s->cmds[i].argv = ft_calloc((s->cmds[i].argc + 1), sizeof(char *));
//             if (!s->cmds[i].argv)
//             {
//                 status = error_return("", errno, 1);
//                 free_commands(s->cmds, s->cmd_count);
//                 free_tokens(&tk);
//                 ft_free(line_buffer);
//                 continue;
//             }
//             int j = 0;
//             while (tk && tk->type != TK_PIPE)
//             {
//                 if (tk->type == TK_NAME)
//                     s->cmds[i].argv[j++] = ft_strdup(tk->content);
//                 else if (tk->type == TK_I && redir(tk, &s->cmds[i].in))
//                 {
//                     free_commands(s->cmds, s->cmd_count);
//                     free_tokens(&tk);
//                     ft_free(line_buffer);
//                     status = 1;
//                     break;
//                 }
//                 else if (tk->type == TK_O && redir(tk, &s->cmds[i].out))
//                 {
//                     free_commands(s->cmds, s->cmd_count);
//                     free_tokens(&tk);
//                     ft_free(line_buffer);
//                     status = 1;
//                     break;
//                 }
//                 if (tk->type == TK_I || tk->type == TK_O)
//                     tk = tk->next;
//                 if (tk)
//                     tk = tk->next;
//             }
//             if (tk)
//                 tk = tk->next;
//             s->cmds[i].argv[j] = NULL;
//             i++;
//         }

//         if (parse_commands(tk, s->cmds, 0, 0))
//         {
//             free_tokens(&tk);
//             ft_free(line_buffer);
//             continue;
//         }
//         free_tokens(&tk);
//         status = SUCCESS;

//         ft_free(line_buffer);
//         if (status == 1)
//             continue;
//         else if (status == 2)
//         {
//             ft_putendl_fd("exit", 2);
//             break;
//         }
//         // print_commands(s->cmds, s->cmd_count);
//         if (s->cmd_count > 0)
//             if (execute(s))
//                 break;
//         free_commands(s->cmds, s->cmd_count);
//     }
//     if (s->cmd_count > 0)
//         free_commands(s->cmds, s->cmd_count);
//     return (0);
// }

/// @brief 			Main function MINISHELL
/// @param argc		Number of arguments (NOT USED)
/// @param argv		Arguments (NOT USED)
/// @param envp		Environment variables
/// @return
// int	main(int argc, char **argv, char **envp)
// {
// 	t_script	s;

// 	(void)argc;
// 	(void)argv;
// 	s.envp = envp_init(envp);
// 	update_shlvl(&s);
// 	s.envt = ft_calloc(1, sizeof(char *));
// 	if (!s.envt)
// 	{
// 		error_return("", errno, 1);
// 		return (1);
// 	}
// 	s.envt[0] = NULL;
// 	termios_getter(&s.termios_p);
// 	s.home = env_var_getter("HOME", s.envp, NULL);
// 	g_exit_status = bi_help();
// 	ms_loop(&s);
// 	return (free_array(s.envp, 0));
// }




// typedef enum e_lex_type {
//     TK_I,
//     TK_O,
//     TK_WS,
//     TK_OR,
//     TK_AND,
//     TK_PIPE,
//     TK_NAME,
//     TK_UNKNOWN
// } t_lex_type;
t_token_type get_token_type(const char *str, size_t size) {
    if (size == 2) {
        if (strncmp(str, "<<", 2) == 0) return TK_I;
        if (strncmp(str, ">>", 2) == 0) return TK_O;
    }
    if (size == 1) {
        if (strncmp(str, "<", 1) == 0) return TK_I;
        if (strncmp(str, ">", 1) == 0) return TK_O;
        if (strncmp(str, "|", 1) == 0) return TK_PIPE;
        if (strncmp(str, " ", 1) == 0 || strncmp(str, "\n", 1) == 0 ||
            strncmp(str, "\t", 1) == 0 || strncmp(str, "\r", 1) == 0 ||
            strncmp(str, "\f", 1) == 0 || strncmp(str, "\v", 1) == 0)
            return TK_WS;
    }
    return TK_UNKNOWN;
}

int main(int argc, char **argv, char **envp)
{
    t_script s;
    char *line_buffer;
    t_token *tk;

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
    // g_exit_status = bi_help();

    line_buffer = NULL;
    while (1)
    {
// Initialize basic int
        s.path = NULL;
        s.hd = NULL;
        s.cmd_count = 0;
        s.exit_status = 0;
// Signal processing functions setter
        signal(SIGINT, sig_handler);
        signal(SIGQUIT, SIG_IGN);
// Inlined parser function
        tk = NULL;
        line_buffer = readline("\001\033[1;94m\002minishell > \001\033[0m\002");
        if (!line_buffer)
            line_buffer = ft_strdup("exit");
        add_history(line_buffer);
// Inlined tk_getter function
        // t_ops ptr;
        char *prev = line_buffer;
        while (line_buffer && *line_buffer) {
            t_token_type type = TK_UNKNOWN;
            size_t size = 1;

            // First check for two-character tokens
            if (line_buffer[1]) { // Ensure there's at least two characters left to check
                if (strncmp(line_buffer, "<<", 2) == 0) {
                    type = TK_I;
                    size = 2;
                } else if (strncmp(line_buffer, ">>", 2) == 0) {
                    type = TK_O;
                    size = 2;
                } 
            }

            // If no two-character token was found, check for one-character tokens
            if (type == TK_UNKNOWN) {
                if (strncmp(line_buffer, "<", 1) == 0) {
                    type = TK_I;
                    size = 1;
                } else if (strncmp(line_buffer, ">", 1) == 0) {
                    type = TK_O;
                    size = 1;
                } else if (strncmp(line_buffer, "|", 1) == 0) {
                    type = TK_PIPE;
                    size = 1;
                } else if (isspace(line_buffer[0])) {
                    type = TK_WS;
                    size = 1;
                }
            }

            if (type != TK_UNKNOWN) {
                if (prev != line_buffer) {
                    tk_lst_addback(&tk, tk_addnew(prev, line_buffer - prev, TK_NAME));
                }
                if (type != TK_WS) {
                    tk_lst_addback(&tk, tk_addnew(line_buffer, size, type));
                }
                line_buffer += size;
                prev = line_buffer;
            } else if (*line_buffer == '\"' || *line_buffer == '\'') {
                char *tmp = line_buffer;
                line_buffer = ft_strchr(line_buffer + 1, *line_buffer);  // Search for the next occurrence of the same quote
                if (!line_buffer) {
                    line_buffer = tmp + ft_strlen(tmp);
                    free_tokens(&tk);
                    ft_free(prev);
                    continue;
                }
            } else {
                ++line_buffer;
            }
        }

        if (prev != line_buffer) {
            tk_lst_addback(&tk, tk_addnew(prev, line_buffer - prev, TK_NAME));
        }

// Inline tk_builder logic
        t_token *tk_ptr;
        char *content;

        tk_ptr = tk;
        while (tk_ptr)
        {
            content = tk_ptr->content;
            if (content[0] == '~' && ft_strlen(content) == 1 && s.home)
                tk_ptr->content = ft_strdup(s.home);
            else
            {
                // Inline tk_var_xpd logic
                char **ntks = tk_var_xpd_init(content);
                int spl = -1;
                int i = 0;

                while (content[i])
                {
                    spl++;
                    if (content[i] == '$')
                        ntks[spl] = tk_var_xpd_dolar(content, &i);
                    else if (content[i] == '\"')
                        tk_var_xpd_dquote(content, &ntks, &spl, &i);
                    else if (content[i] == '\'')
                        tk_var_xpd_squote(content, &ntks, &spl, &i);
                    else
                        tk_var_xpd_else(content, &ntks, &spl, &i);
                }

                // Inline tk_xpd_filler logic
                char *tmp = ft_strdup("");
                int j = -1;
                char *tmp2;

                while (ntks[++j])
                {
                    tmp2 = ntks[j];
                    if (ntks[j][0] == '$' && (ntks[j][1] || ntks[j + 1]))
                    {
                        ntks[j] = tk_xpd_var_filler(tmp2, &s);
                        if (!ntks[j])
                            ntks[j] = ft_strdup("");
                    }
                    else
                        ntks[j] = tk_xpd_unquote(tmp2);
                    char *new_tmp = ft_strjoin_free(tmp, ntks[j]);
                    tmp = new_tmp;
                }
                
                tk_ptr->content = tmp;  // Set the content to expanded value
                free(ntks);  // Free the tokens array
            }
            ft_free(content);
            tk_ptr = tk_ptr->next;
        }


// Inline tk_rm_blank logic
        if (tk)
        {
            t_token **tk_p = &tk;
            t_token *tk_n = tk;
            char c;

            while (tk_n)
            {
                if (tk_n->type == TK_NAME && tk_n->rm)
                {
                    char *str1 = tk_n->content;
                    char *str2;
                    char *tmp;
                    t_token *tk_new;

                    c = ft_strset(str1, " \t\v\r\n\f");
                    if (c)
                    {
                        //tk_blank_split logic
                        *tk_p = NULL;
                        while (str1 && *str1 && c)
                        {
                            tk_new = tk_addnew(str1, ft_strchr(str1, c) - str1, TK_NAME);
                            tk_lst_addback(tk_p, tk_new);
                            str2 = ft_strtrim(ft_strchr(str1, c), " \t\v\r\n\f");
                            if (ft_strncmp(str1, tk_n->content, ft_strlen(tk_n->content)))
                                ft_free(str1);
                            str1 = str2;
                            c = ft_strset(str1, " \t\v\r\n\f");
                        }
                        tmp = tk_n->content;
                        tk_n->content = str1;
                        tk_n->size = ft_strlen(str1);
                        tk_new->next = tk_n;
                        ft_free(tmp);
                    }
                }
                tk_p = &(tk_n->next);
                tk_n = tk_n->next;
            }
        }

// Inline syntax_checker logic
        t_token *tk_checker = tk;
        if (tk_checker && tk_checker->type == TK_PIPE)
        {
            syntax_error_msg(tk_checker->content, 2, 0);
            free_tokens(&tk);
            ft_free(line_buffer);
            continue;
        }
        while (tk_checker)
        {
            if (!tk_checker->next && (tk_checker->type == TK_PIPE || tk_checker->type == TK_OR))
            {
                syntax_error_msg(tk_checker->content, 2, 0);
                free_tokens(&tk);
                ft_free(line_buffer);
                continue;
            }
            if (!tk_checker->next && (tk_checker->type == TK_I || tk_checker->type == TK_O))
            {
                syntax_error_msg("newline", 2, 0);
                free_tokens(&tk);
                ft_free(line_buffer);
                continue;
            }
            if (tk_checker->next && ((tk_checker->type == TK_PIPE && tk_checker->next->type == TK_PIPE)
                    || (tk_checker->type == TK_PIPE && tk_checker->next->type == TK_OR)
                    || (tk_checker->type == TK_OR && tk_checker->next->type == TK_OR)
                    || (tk_checker->type == TK_OR && tk_checker->next->type == TK_PIPE)))
            {
                syntax_error_msg(tk_checker->next->content, 2, 0);
                free_tokens(&tk);
                ft_free(line_buffer);
                continue;
            }
            if ((tk_checker->type == TK_O || tk_checker->type == TK_I)
                && (tk_checker->next && tk_checker->next->type != TK_NAME))
            {
                syntax_error_msg(tk_checker->next->content, 2, 0);
                free_tokens(&tk);
                // ft_free(line_buffer);
                continue;
            }
            if ((tk_checker->type == TK_O || tk_checker->type == TK_I)
                && (tk_checker->next && (!ft_strncmp(tk_checker->next->content, "&", 1))))
            {
                syntax_error_msg("newline", 2, 0);
                free_tokens(&tk);
                ft_free(line_buffer);
                continue;
            }
            tk_checker = tk_checker->next;
        }

// Inline cmds_counter logic
        s.cmd_count = 0;
        t_token *tk_count = tk;
        while (tk_count)
        {
            if (!s.cmd_count)
                s.cmd_count = 1;
            if (tk_count->type == TK_PIPE)
                s.cmd_count++;
            tk_count = tk_count->next;
        }

        s.cmds = ft_calloc(s.cmd_count, sizeof(t_command));
        if (!s.cmds)
        {
            error_return("", errno, 1);
            free_tokens(&tk);
            ft_free(line_buffer);
            continue;
        }
        if (s.cmd_count == 0)
        {
            free_commands(s.cmds, s.cmd_count);
            free_tokens(&tk);
            // ft_free(line_buffer);
            continue;
        }

// Inline args_counter logic
        t_token *tmp;
        int i = 0;
        t_token *tk_arg_counter = tk;
        while (i < s.cmd_count)
        {
            s.cmds[i].argc = 0;
            tmp = tk_arg_counter;
            while (tk_arg_counter && tk_arg_counter->type != TK_PIPE)
            {
                if (tk_arg_counter->type == TK_NAME && (tmp->type != TK_I && tmp->type != TK_O))
                    s.cmds[i].argc++;
                tmp = tk_arg_counter;
                tk_arg_counter = tk_arg_counter->next;
            }
            if (tk_arg_counter)
                tk_arg_counter = tk_arg_counter->next;
            i++;
        }

// Inline fnames_clear logic
        for (i = 0; i < s.cmd_count; i++)
        {
            s.cmds[i].in.name = NULL;
            s.cmds[i].out.name = NULL;
            s.cmds[i].in.heredoc = NULL;
            s.cmds[i].in.flag = -1;
            s.cmds[i].out.flag = -1;
        }
// Inlined parse_commands logic
        i = 0;
        t_token *tk_parse = tk;
        while (tk_parse) {
            s.cmds[i].argv = ft_calloc((s.cmds[i].argc + 1), sizeof(char *));
            if (!s.cmds[i].argv) {
                free_commands(s.cmds, i + 1);
                free_tokens(&tk);
                ft_free(line_buffer);
                return (error_return("", errno, 1));
            }
            int j = 0;
            while (tk_parse && tk_parse->type != TK_PIPE) {
                if (tk_parse->type == TK_NAME) {
                    s.cmds[i].argv[j++] = ft_strdup(tk_parse->content);
                } else if (tk_parse->type == TK_I || tk_parse->type == TK_O) {
                    t_redirection *file = (tk_parse->type == TK_I) ? &s.cmds[i].in : &s.cmds[i].out;
                    int ret;

                    if (file->name)
                        free(file->name);

                    if (!tk_parse->next || tk_parse->next->type != TK_NAME) {
                        free_commands(s.cmds, i + 1);
                        free_tokens(&tk);
                        ft_free(line_buffer);
                        return (SUCCESS);
                    }

                    file->name = ft_strdup(tk_parse->next->content);

                    if (!ft_strncmp(tk_parse->content, ">>", 2) || !ft_strncmp(tk_parse->content, "<>", 2)) {
                        file->flag = (O_CREAT | O_APPEND | O_RDWR);
                    } else if (!ft_strncmp(tk_parse->content, "<<", 2)) {
                        fill_heredoc(file);  // Assuming this function handles setting up the heredoc
                    } else if (!ft_strncmp(tk_parse->content, ">", 1) || !ft_strncmp(tk_parse->content, ">|", 2)) {
                        file->flag = (O_CREAT | O_TRUNC | O_RDWR);
                    } else if (!ft_strncmp(tk_parse->content, "<", 1)) {
                        file->flag = O_RDONLY;
                    }

                    if (file->flag != -1) {
                        ret = open(file->name, file->flag, 0644);
                        if (ret == -1) {
                            free_commands(s.cmds, i + 1);
                            free_tokens(&tk);
                            ft_free(line_buffer);
                            return (redir_error(file->name));
                        }
                        close(ret);
                    }

                    tk_parse = tk_parse->next;  // Skip the filename
                }

                if (tk_parse)
                    tk_parse = tk_parse->next;
            }
            if (tk_parse)
                tk_parse = tk_parse->next;
            s.cmds[i].argv[j] = NULL;
            i++;
        }


        print_commands(s.cmds, s.cmd_count);
        if (s.cmd_count > 0)
            if (execute(&s))
                break;
        free_commands(s.cmds, s.cmd_count);
    }
    if (s.cmd_count > 0)
        free_commands(s.cmds, s.cmd_count);

    return (free_array(s.envp, 0));
}
