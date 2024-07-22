/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rerodrig <rerodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/10 02:02:08 by nnuno-ca          #+#    #+#             */
/*   Updated: 2024/07/12 12:38:40 by rerodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./includes/minishell.h"
#include <signal.h>

long long	g_exit_status = 0;

// static size_t	expand_exit_status(char *expanded_input_at_i, size_t *i)
// {
// 	char	*exit_status;
// 	size_t	j;

// 	*i += 2;
// 	exit_status = ft_lltoa(g_exit_status);
// 	j = 0;
// 	while (exit_status[j])
// 	{
// 		expanded_input_at_i[j] = exit_status[j];
// 		j += 1;
// 	}
// 	free(exit_status);
// 	return (j);
// }

// static size_t	expand_variable(char *expanded_input_at_i, char *input,
// 	size_t *i, t_data *data)
// {
// 	char	*var_value;
// 	size_t	size;
// 	size_t	j;
// 	size_t	k;

// 	size = 0;
// 	j = 0;
// 	k = 0;
// 	*i += 1;
// 	if (!input[*i] || input[*i] == ' ' || input[*i] == '\"')
// 	{
// 		expanded_input_at_i[0] = '$';
// 		return (1);
// 	}
// 	while (input[*i + size] && input[*i + size] != ' '
// 		&& input[*i + size] != '\"' && !is_onstr(QUOTES, input[*i + size])
// 		&& input[*i + size] != '$')
// 		size += 1;
// 	var_value = get_varvalue_fromvlst(ft_substr(input, *i, size), data);
// 	*i += size;
// 	if (!var_value)
// 		return (0);
// 	while (var_value[k])
// 		expanded_input_at_i[j++] = var_value[k++];
// 	return (j);
// }

static char	*get_input(void)
{
	char	*raw_input;
	char	*input;

	raw_input = readline("minishell$ ");
	input = trim_free(raw_input, " \t");
	return (input);
}

static void	clean_parsed(t_statement **statement_list, t_data *data)
{
	p_lstclear(statement_list);
	*statement_list = NULL;
	data->head = NULL;
}


static size_t	remove_quotes_size(char	*parsed)
{
	size_t	i;
	size_t	size;
	char	quotes;

	i = 0;
	size = 0;
	while (parsed[i])
	{
		while (parsed[i] && !is_onstr("\"\'", parsed[i]))
		{
			i += 1;
			size += 1;
		}
		if (!parsed[i])
			break ;
		quotes = parsed[i++];
		while (parsed[i] && parsed[i] != quotes)
		{
			i += 1;
			size += 1;
		}
		quotes = 0;
	}
	return (size);
}

int	main(int ac, char **av, char **envp)
{
	t_data		data;
	t_statement	*statement_list;
	char		*input;

	if (av && ac > 1)
		panic(NULL, "minishell: no support for command-line arguments", EXIT_FAILURE);
	setup_shell(envp, &data, &statement_list);
	
	while (1)
	{
		input = get_input();

		// if (!valid_input(input, &data))
			bool valid = true;
			char last_opened;
			size_t i;
			bool in_quotes;

			if (input == NULL)
			{
				free(input);
				// exit_shell(EXIT_SUCCESS, data);
				ft_putendl_fd("exit", STDOUT_FILENO);
				g_exit_status = EXIT_SUCCESS;
				if (data.head != NULL)
					destroy(&data);
				exit(g_exit_status);
			}
			if (input[0] == '\0')
			{
				valid = false;
			}
			else
			{
				// Inlining the unclosed_quotes function
				last_opened = 0;
				for (char *str = input; *str;)
				{
					while (*str && !last_opened)
					{
						if (*str == '\'' || *str == '"')
							last_opened = *str;
						str++;
					}
					while (*str && last_opened)
					{
						if (*str && *str == last_opened)
							last_opened = 0;
						str++;
					}
					if (*str)
					{
						last_opened = 0; 
					}
					else if (last_opened)
					{
						valid = false;
						break;
					}
				}
				if (!valid)
				{
					ft_putendl_fd("minishell: unclosed quotes", STDERR_FILENO);
				}
				else
				{
					// Inlining invalid_syntax
					if (input[0] == '|')
					{
						// unexpected_token('|');
						ft_putstr_fd("minishell: syntax error near unexpected token `", STDERR_FILENO);
						ft_putchar_fd('|', STDERR_FILENO);
						ft_putendl_fd("'", STDERR_FILENO);
						valid = false;
					}
					else if (input[ft_strlen(input) - 1] == '|')
					{
						ft_putendl_fd("minishell: no support for pipe prompt", STDERR_FILENO);
						valid = false;
					}
					else if (is_onstr("><", input[ft_strlen(input) - 1]))
					{
						ft_putendl_fd("minishell: syntax error near unexpected token `newline'", STDERR_FILENO);
						valid = false;
					}

					// Inlining invalid_syntax2
					i = 0;
					in_quotes = false;
					while (valid && input[i])
					{
						if (is_onstr("\"\'", input[i]))
							in_quotes = !in_quotes;
						if (((input[i] == '>' && input[i + 1] == '<') ||
							(input[i] == '<' && input[i + 1] == '>') ||
							(input[i] == '|' && input[i + 1] == '|')) && !in_quotes)
						{
							// unexpected_token(input[i + 1]);
							ft_putstr_fd("minishell: syntax error near unexpected token `", STDERR_FILENO);
							ft_putchar_fd(input[i + 1], STDERR_FILENO);
							ft_putendl_fd("'", STDERR_FILENO);
								
							valid = false;	
						}
						else if ((input[i] == '{' || input[i] == '}' ||
								input[i] == '(' || input[i] == ')' ||
								input[i] == '[' || input[i] == ']' ||
								input[i] == ';' || input[i] == '&' || input[i] == '*') && !in_quotes)
						{
							// unexpected_token(input[i]);
							ft_putstr_fd("minishell: syntax error near unexpected token `", STDERR_FILENO);
							ft_putchar_fd(input[i], STDERR_FILENO);
							ft_putendl_fd("'", STDERR_FILENO);
							valid = false;
						}
						i += 1;
					}

					// Inlining invalid_syntax_on_operator
					i = 0;
					in_quotes = false;
						bool has_operator;
						i = 0;
						while (input[i])
						{
							if (is_onstr("|<>", input[i]))
								has_operator=true;
							i += 1;
						}
						has_operator=false;
					while (valid && has_operator)
					{
						if (is_onstr("\"\'", input[i]))
							in_quotes = !in_quotes;
						if (is_onstr("|<>", input[i]) && !in_quotes)
						{
							if (input[i] == input[i + 1])
								i += 2;
							else
								i += 1;
							if (input[i] == ' ')
							{
								while (input[i] && input[i] == ' ')
									i += 1;
								if (is_onstr("|<>", input[i]))
								{
									ft_putstr_fd("minishell: syntax error near unexpected token `", STDERR_FILENO);
									ft_putchar_fd(input[i], STDERR_FILENO);
									ft_putendl_fd("'", STDERR_FILENO);
									valid = false;
								}
							}
							if (valid && is_onstr("|<>", input[i]))
							{
								ft_putstr_fd("minishell: syntax error near unexpected token `", STDERR_FILENO);
								ft_putchar_fd(input[i], STDERR_FILENO);
								ft_putendl_fd("'", STDERR_FILENO);
								valid = false;
							}
						}
						i += 1;
					}
				}
			}

			if (!valid)
			{
				free(input);
				g_exit_status = 2;
			}
			
		if (!valid)
			continue ;
		add_history(input);
		input = expander(input, &data);
		if (!input[0])
		{
			free(input);
			continue ;
		}
		// // input = expander(input, &data);
		// 	size_t	i;
		// 	size_t	j;
		// 	bool	in_quotes;
		// 	bool	in_dquotes;
		// 	char	*expanded_input;

		// 	init_vars(&i, &j, &in_quotes, &in_dquotes);
		// 	expanded_input = malloc((expanded_size(input, &data) + 1) * sizeof(char));
		// 	while (input[i])
		// 	{
		// 		if (input[i] == '\"' && !in_quotes)
		// 			in_dquotes = !in_dquotes;
		// 		if (input[i] == '\'' && !in_dquotes)
		// 			in_quotes = !in_quotes;
		// 		if (input[i] == '$' && input[i + 1] == '?' && !in_quotes)
		// 			j += expand_exit_status(&(expanded_input[j]), &i);
		// 		else if (input[i] && input[i] == '$' && !in_quotes)
		// 			j += expand_variable(&(expanded_input[j]), input, &i, &data);
		// 		else
		// 			expanded_input[j++] = input[i++];
		// 	}
		// 	expanded_input[j] = '\0';
		// 	// free(input);
		// if (!input[0])
		// {
		// 	free(input);
		// 	continue ;
		// }
		// statement_list = parser(input);

			char		**parsed;
			t_statement	*temp;
			t_statement	*head;
			size_t		idx[2];

			// parsed = parse_input(input);
				// size_t		i;
				size_t		k;
				char		**parse_input;
				size_t		len;
				size_t		j;

				i = 0;
				k = 0;

				// parse_input = malloc((get_nr_statements(input) + 1) * sizeof(char *));
					size_t	count;
					bool	flag;
					bool	quotes;

					count = 0;
					flag = false;
					quotes = false;
					while (input[i])
					{
						if (is_onstr("|<>", input[i]))
							count += 1;
						if (is_onstr("\"\'", input[i]) && input[i] == *(input + 1))
							input += 1;
						else if (is_onstr("\"\'", input[i]))
							quotes = !quotes;
						if (input[i] != ' ' && !is_onstr("|<>", input[i]) && !flag && !quotes)
						{
							flag = true;
							count += 1;
						}
						else if (input[i] == ' ' || is_onstr("|<>", input[i]))
							flag = false;
						i += 1;
					}
				// parse_input = malloc((get_nr_statements(input) + 1) * sizeof(char *));
				parse_input = malloc((count + 1) * sizeof(char *));
				i= 0;
				while (input[i])
				{
					// len = get_token_len(&input[i]);
						size_t	x;
						char	quotes;

						x = 0;
						if (is_onstr("|<>", input[x]))
						{
							if (input[x] == input[x + 1])
								return (2);
							return (1);
						}
						while (input[x]
							&& !is_spaces(input[x])
							&& !is_onstr("|<>", input[x]))
						{
							if (is_onstr("\"\'", input[x]))
							{
								quotes = input[x++];
								while (input[x] && input[x] != quotes)
									x += 1;
							}
							x += 1;
						}
					// len = get_token_len(&input[i]);
					len = x;
						
					if (!len)
					{
						i += 1;
						continue ;
					}
					parse_input[k] = malloc((len + 1) * sizeof(char));
					j = 0;
					while (input[i] && j < len)
						parse_input[k][j++] = input[i++];
					parse_input[k++][j] = '\0';
				}
				parse_input[k] = NULL;
				parsed = parse_input;
			// parsed = parse_input(input);
			// free(input);
			// temp = p_new_node(get_argc(&parsed[0]));
				size_t	y;

				y = 0;
				while (parsed[0][y])
					y += 1;
			// temp = p_new_node(get_argc(&parsed[0]));
			temp = p_new_node(y);
			head = temp;
			idx[0] = 0;
			while (parsed[idx[0]])
			{
				idx[1] = 0;
				// while (parsed[idx[0]] && !is_onstr(OPERATORS, parsed[idx[0]][0]))
				// 	temp->argv[idx[1]++] = remove_quotes(parsed[idx[0]++]);
					while (parsed[idx[0]] && !is_onstr("|<>", parsed[idx[0]][0])) {
						size_t a = 0, b = 0;
						char quotes = '\0';
						char *unquoted_parsed;
						
						// Calculate the size needed for unquoted_parsed
						size_t size = remove_quotes_size(parsed[idx[0]]);
						unquoted_parsed = malloc((size + 1) * sizeof(char));

						// Process the current parsed string to remove quotes
						while (parsed[idx[0]][a]) {
							// Copy non-quote characters
							while (parsed[idx[0]][a] && !is_onstr("\"\'", parsed[idx[0]][a])) {
								unquoted_parsed[b++] = parsed[idx[0]][a++];
							}
							if (!parsed[idx[0]][a])
								break;

							// Handle quoted characters
							quotes = parsed[idx[0]][a];
							a++;
							while (parsed[idx[0]][a] && parsed[idx[0]][a] != quotes) {
								unquoted_parsed[b++] = parsed[idx[0]][a++];
							}
							quotes = '\0';
							a++;
						}
						unquoted_parsed[b] = '\0';

						// Assign unquoted_parsed to temp->argv[idx[1]]
						temp->argv[idx[1]++] = unquoted_parsed;

						// Move to the next parsed element
						idx[0]++;
					}
				temp->argv[idx[1]] = NULL;
				if (!parsed[idx[0]])
					break ;
				// temp->operator = get_operator(parsed[idx[0]++]);
					    // Inline get_operator logic
						if (streq(parsed[idx[0]], "|"))
							temp->operator = PIPE;
						else if (streq(parsed[idx[0]], ">>"))
							temp->operator = RDR_OUT_APPEND;
						else if (streq(parsed[idx[0]], ">"))
							temp->operator = RDR_OUT_REPLACE;
						else if (streq(parsed[idx[0]], "<<"))
							temp->operator = RDR_INPUT_UNTIL;
						else if (streq(parsed[idx[0]], "<"))
							temp->operator = RDR_INPUT;
						else
							temp->operator = NONE;

				// temp->operator = get_operator(parsed[idx[0]++]);
				free(parsed[idx[0]]);
				idx[0]++;


				// temp->next = p_new_node(get_argc(&parsed[idx[1]]));
					    // Inline get_argc logic
						size_t argc = 0;
						while (parsed[idx[0] + argc] && !is_onstr("|<>", parsed[idx[0] + argc][0])) {
							argc++;
						}
				// temp->next = p_new_node(get_argc(&parsed[idx[1]]));
				temp->next = p_new_node(argc);
				temp = temp->next;
			}
			
			
			temp->next = NULL;
			free(parsed);
			statement_list = head;




		data.head = statement_list;
		exec_type(statement_list, &data);
		clean_parsed(&statement_list, &data);
	}
	return (EXIT_SUCCESS);
}
// int	main(int ac, char **av, char **envp)
// {
// 	t_data		data;
// 	t_statement	*statement_list;
// 	char		*input;

// 	if (av && ac > 1)
// 		panic(NULL, CL_ARGUMENTS_ERR, EXIT_FAILURE);
// 	setup_shell(envp, &data, &statement_list);
// 	while (1)
// 	{
// 		input = get_input();
// 		if (!valid_input(input, &data))
// 			continue ;
// 		add_history(input);
// 		// input = expander(input, &data);
// 		size_t	i;
// 		size_t	d;
// 		bool	in_quotes;
// 		bool	in_dquotes;
// 		char	*expanded_input;

// 		init_vars(&i, &d, &in_quotes, &in_dquotes);
// 		expanded_input = malloc((expanded_size(input, &data) + 1) * sizeof(char));
// 		while (input[i])
// 		{
// 			if (input[i] == '\"' && !in_quotes)
// 				in_dquotes = !in_dquotes;
// 			if (input[i] == '\'' && !in_dquotes)
// 				in_quotes = !in_quotes;
// 			if (input[i] == '$' && input[i + 1] == '?' && !in_quotes)
// 				d += expand_exit_status(&(expanded_input[d]), &i);
// 			else if (input[i] && input[i] == '$' && !in_quotes)
// 				d += expand_variable(&(expanded_input[d]), input, &i, &data);
// 			else
// 				expanded_input[d++] = input[i++];
// 		}
// 		expanded_input[d] = '\0';
// 		free(input);
// 		if (!expanded_input[0])
// 		{
// 			free(expanded_input);
// 			continue ;
// 		}
// 		// statement_list = parser(expanded_input);
// 		char		**parsed;
// 		t_statement	*temp;
// 		t_statement	*head;
// 		size_t		idx[2];

// 		// parsed = parse_input(expanded_input);
// 		size_t		k;
// 		char		**parse_input;
// 		size_t		len;
// 		size_t		j;

// 		i = 0;
// 		k = 0;

// 		// parse_input = malloc((get_nr_statements(expanded_input) + 1) * sizeof(char *));
// 		size_t	count;
// 		bool	flag;
// 		bool	quotes;

// 		count = 0;
// 		flag = false;
// 		quotes = false;
// 		const char *input_ptr = expanded_input; // temporary pointer to not move the original pointer
// 		while (*input_ptr)
// 		{
// 			if (is_onstr(OPERATORS, *input_ptr))
// 				count += 1;
// 			if (is_onstr(QUOTES, *input_ptr) && *input_ptr == *(input_ptr + 1))
// 				input_ptr += 1;
// 			else if (is_onstr(QUOTES, *input_ptr))
// 				quotes = !quotes;
// 			if (*input_ptr != ' ' && !is_onstr(OPERATORS, *input_ptr) && !flag && !quotes)
// 			{
// 				flag = true;
// 				count += 1;
// 			}
// 			else if (*input_ptr == ' ' || is_onstr(OPERATORS, *input_ptr))
// 				flag = false;
// 			input_ptr += 1;
// 		}

// 		parse_input = malloc((count + 1) * sizeof(char *));
// 		while (expanded_input[i])
// 		{
// 			// len = get_token_len(&expanded_input[i]);
// 			size_t	x;
// 			char	quotes;

// 			x = 0;
// 			if (is_onstr(OPERATORS, expanded_input[x]))
// 			{
// 				if (expanded_input[x] == expanded_input[x + 1])
// 					len = 2;
// 				else
// 					len = 1;
// 			}
// 			else
// 			{
// 				while (expanded_input[x]
// 					&& !is_spaces(expanded_input[x])
// 					&& !is_onstr(OPERATORS, expanded_input[x]))
// 				{
// 					if (is_onstr(QUOTES, expanded_input[x]))
// 					{
// 						quotes = expanded_input[x++];
// 						while (expanded_input[x] && expanded_input[x] != quotes)
// 							x += 1;
// 					}
// 					x += 1;
// 				}
// 				len = x;
// 			}
				
// 			if (!len)
// 			{
// 				i += 1;
// 				continue ;
// 			}
// 			parse_input[k] = malloc((len + 1) * sizeof(char));
// 			j = 0;
// 			while (expanded_input[i] && j < len)
// 				parse_input[k][j++] = expanded_input[i++];
// 			parse_input[k++][j] = '\0';
// 		}
// 		parse_input[k] = NULL;
// 		parsed = parse_input;
// 		free(expanded_input);
// 		// temp = p_new_node(get_argc(&parsed[0]));
// 		size_t	y;

// 		y = 0;
// 		while (parsed[0][y])
// 			y += 1;
// 		temp = p_new_node(y);
// 		head = temp;
// 		idx[0] = 0;
// 		while (parsed[idx[0]])
// 		{
// 			idx[1] = 0;
// 			while (parsed[idx[0]] && !is_onstr(OPERATORS, parsed[idx[0]][0])) {
// 				size_t a = 0, b = 0;
// 				char quotes = '\0';
// 				char *unquoted_parsed;
				
// 				// Calculate the size needed for unquoted_parsed
// 				size_t size = 0;
// 				{
// 					size_t	i;
// 					char	quotes;

// 					i = 0;
// 					while (parsed[idx[0]][i])
// 					{
// 						while (parsed[idx[0]][i] && !is_onstr(QUOTES, parsed[idx[0]][i]))
// 						{
// 							i += 1;
// 							size += 1;
// 						}
// 						if (!parsed[idx[0]][i])
// 							break ;
// 						quotes = parsed[idx[0]][i++];
// 						while (parsed[idx[0]][i] && parsed[idx[0]][i] != quotes)
// 						{
// 							i += 1;
// 							size += 1;
// 						}
// 						quotes = 0;
// 					}
// 				}

// 				unquoted_parsed = malloc((size + 1) * sizeof(char));

// 				// Process the current parsed string to remove quotes
// 				while (parsed[idx[0]][a]) {
// 					// Copy non-quote characters
// 					while (parsed[idx[0]][a] && !is_onstr(QUOTES, parsed[idx[0]][a])) {
// 						unquoted_parsed[b++] = parsed[idx[0]][a++];
// 					}
// 					if (!parsed[idx[0]][a])
// 						break;

// 					// Handle quoted characters
// 					quotes = parsed[idx[0]][a];
// 					a++;
// 					while (parsed[idx[0]][a] && parsed[idx[0]][a] != quotes) {
// 						unquoted_parsed[b++] = parsed[idx[0]][a++];
// 					}
// 					quotes = '\0';
// 					a++;
// 				}
// 				unquoted_parsed[b] = '\0';

// 				// Assign unquoted_parsed to temp->argv[idx[1]]
// 				temp->argv[idx[1]++] = unquoted_parsed;

// 				// Move to the next parsed element
// 				idx[0]++;
// 			}
// 			temp->argv[idx[1]] = NULL;
// 			if (!parsed[idx[0]])
// 				break ;
// 			// temp->operator = get_operator(parsed[idx[0]++]);
// 			if (streq(parsed[idx[0]], "|"))
// 				temp->operator = PIPE;
// 			else if (streq(parsed[idx[0]], ">>"))
// 				temp->operator = RDR_OUT_APPEND;
// 			else if (streq(parsed[idx[0]], ">"))
// 				temp->operator = RDR_OUT_REPLACE;
// 			else if (streq(parsed[idx[0]], "<<"))
// 				temp->operator = RDR_INPUT_UNTIL;
// 			else if (streq(parsed[idx[0]], "<"))
// 				temp->operator = RDR_INPUT;
// 			else
// 				temp->operator = NONE;

// 			free(parsed[idx[0]]);
// 			idx[0]++;

// 			// temp->next = p_new_node(get_argc(&parsed[idx[1]]));
// 			size_t argc = 0;
// 			while (parsed[idx[0] + argc] && !is_onstr(OPERATORS, parsed[idx[0] + argc][0])) {
// 				argc++;
// 			}
// 			temp->next = p_new_node(argc);
// 			temp = temp->next;
// 		}
		
		
// 		temp->next = NULL;
// 		free(parsed);
// 		statement_list = head;

// 		data.head = statement_list;
// 		exec_type(statement_list, &data);
// 		clean_parsed(&statement_list, &data);
// 	}
// 	return (EXIT_SUCCESS);
// }
