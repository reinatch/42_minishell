/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amitcul <amitcul@student.42porto.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/16 20:13:05 by amitcul           #+#    #+#             */
/*   Updated: 2023/06/05 17:30:59 by amitcul          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"


void	count_pipes(t_app *app)
{
	t_lexer_token	*tmp;

	tmp = app->lexer_tokens;
	app->pipes_count = 0;
	while (tmp)
	{
		if (tmp->token_type == PIPE)
			app->pipes_count += 1;
		tmp = tmp->next;
	}
}

t_parser	*init_parser(t_app *app)
{
	t_parser	*parser;

	parser = (t_parser *)malloc(sizeof(t_parser));
	parser->app = app;
	parser->lexer_list = app->lexer_tokens;
	parser->redirs = NULL;
	parser->redirs_count = 0;
	return (parser);
}

// int	parser(t_app *app)
// {
// 	t_parser	*parser;
// 	t_command	*command;

// 	parser = NULL;
// 	if (app->lexer_tokens->token_type == PIPE)
// 		 return (parser_double_token_error(app, app->lexer_tokens,
// 		 		app->lexer_tokens->token_type));
// 	count_pipes(app);
// 	while (app->lexer_tokens)
// 	{
// 		if (app->lexer_tokens->token_type == PIPE)
// 			delete_node_by_index(&app->lexer_tokens, app->lexer_tokens->index);
// 		if (handle_pipe_errors(app, app->lexer_tokens->token_type))
// 			return (EXIT_FAILURE);
// 		parser = init_parser(app);
// 		command = get_command(parser);
// 		if (!command)
// 			parser_error(0, app, parser->lexer_list);
// 		if (!app->commands_list)
// 			app->commands_list = command;
// 		else
// 			add_command_to_list(app, command);
// 		app->lexer_tokens = parser->lexer_list;
// 	}
// 	return (EXIT_SUCCESS);
// }
int loop(t_app *app);
static int reset(t_app *app)
{
	/*
	reset and clear all structs
	*/
	loop(app);
	return (EXIT_SUCCESS);
}
static t_command	*init_command(char **str, size_t redirs_count,
		t_lexer_token *redirs)
{
	t_command	*command;

	command = (t_command *)malloc(sizeof(t_command));
	if (!command)
		return (NULL);
	command->str = str;
	command->redirs_count = redirs_count;
	command->redirs = redirs;
	command->next = NULL;
	command->prev = NULL;
	return (command);
}
static size_t	count_args(t_lexer_token *list)
{
	size_t	count;

	count = 0;
	while (list && list->token_type != PIPE)
	{
		count += 1;
		list = list->next;
	}
	return (count);
}

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

int parser(t_app *app)
{
    t_parser    *parser;
    t_command   *command;

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

        // Inline get_command
        collect_redirections(parser);
        size_t args_count = count_args(parser->lexer_list);
        char **str = (char **) ft_calloc(args_count + 1, sizeof(char *));
        if (!str)
        {
            ft_error(2, app);
            return (EXIT_FAILURE);
        }
        t_lexer_token *curr = parser->lexer_list;
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
        command = init_command(str, parser->redirs_count, parser->redirs);
        if (!command)
        {
            // Inline parser_error
            ft_lexerclear(&parser->lexer_list);
            ft_error(0, app);
            return (EXIT_FAILURE);
        }

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
    }

    return (EXIT_SUCCESS);
}
