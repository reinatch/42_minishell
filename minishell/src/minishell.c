

#include "../include/minishell.h"

// Global exit status
int g_exit = 0;

// Function to create a new token
Token *new_token(TokenType type, char **value) {
    Token *token = (Token *)malloc(sizeof(Token));
    if (!token) return NULL; // Check for allocation failure

    token->type = type;

    // Calculate the size of the value array
    size_t count = 0;
    while (value && value[count]) {
        count++;
    }

    // Allocate memory for to_execute array
    token->to_execute = (char **)malloc((count + 1) * sizeof(char *));
    if (!token->to_execute) {
        free(token);
        return NULL; // Handle allocation failure
    }

    // Copy each string into the to_execute array
    for (size_t i = 0; i < count; i++) {
        token->to_execute[i] = ft_strdup(value[i]);
        if (!token->to_execute[i]) {
            // Free previously allocated memory if strdup fails
            while (i > 0) {
                free(token->to_execute[--i]);
            }
            free(token->to_execute);
            free(token);
            return NULL;
        }
    }
    token->to_execute[count] = NULL; // Null-terminate the array

    // Initialize other fields
    token->input_file = NULL;
    token->output_file = NULL;
    token->append = 0;
    token->heredoc_delimiter = NULL;
    token->heredoc_content = NULL;
    token->pipe_to_next_token = 0;
    token->next = NULL;

    return token;
}

// Function to free the token list
void free_token_list(Token *head) {
    Token *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;

        // Free each string in the to_execute array
        if (temp->to_execute) {
            int i = 0;
            while (temp->to_execute[i]) {
                free(temp->to_execute[i]);
                i++;
            }
            free(temp->to_execute);
        }

        // Free other dynamically allocated strings
        if (temp->input_file) {
            free(temp->input_file);
        }
        if (temp->output_file) {
            free(temp->output_file);
        }

        // Free the Token struct itself
        free(temp);
    }
}

// Function to find the next quote in the string
int find_next_quote(char *line, int start, int *count, char quote) {
    int i = start + 1;
    *count += 1;
    while (line[i] && line[i] != quote) {
        i++;
    }
    if (line[i] == quote) {
        *count += 1;
    }
    return (i - start);
}

// Function to check for balanced quotes
int check_quotes(char *line) {
    int i = 0;
    int double_quote_count = 0;
    int single_quote_count = 0;
    while (line[i]) {
        if (line[i] == '\'') {
            i += find_next_quote(line, i, &single_quote_count, '\'');
        } else if (line[i] == '"') {
            i += find_next_quote(line, i, &double_quote_count, '"');
        } else {
            i++;
        }
    }
    return (double_quote_count % 2 == 0) && (single_quote_count % 2 == 0);
}

// Function to print error messages
void print_error(char *msg, char *key, int exit_code) {
    if (key) {
        if (msg) {
            ft_printf("%s: %s: %s\n", ERROR_TITLE, key, msg);
        } else {
            ft_printf("%s: %s `%s'\n", ERROR_TITLE, ERROR_SYNTAX, key);
        }
    } else {
        ft_printf("%s: %s\n", ERROR_TITLE, msg);
    }
    g_exit = exit_code;
}






// Function to check if a character is present in a string
int ft_chkchar(const char *str, int ch) {
    size_t i;

    if (!str) {
        return 0;
    }
    i = 0;
    while (str[i]) {
        if (str[i] == ch) {
            return 1;
        }
        i += 1;
    }
    return 0;
}


void loop_heredoc(char *delimiter, int pipe_fd, char **hd_content) {
    char *line;
    char *new_content;

    *hd_content = ft_strdup("");  // Initialize heredoc content as an empty string

    while (1) {
        line = readline("> ");  // Read input line by line
        if (!line || !ft_strncmp(line, delimiter, ft_strlen(delimiter) + 1)) {
            // Check if the line is null or matches the delimiter (indicating the end of heredoc)
            if (!line) {
                print_error("Error reading heredoc input", NULL, 1);
            }
            free(line);
            break;
        }

        // Append a newline character to the input line
        new_content = ft_strjoin(line, "\n");

        // Concatenate the new content to the existing heredoc content
        char *old_content = *hd_content;
        *hd_content = ft_strjoin(old_content, new_content);

        // Free temporary variables to avoid memory leaks
        free(old_content);
        free(new_content);
        free(line);
    }

    // Write the entire heredoc content to the pipe
    if (write(pipe_fd, *hd_content, ft_strlen(*hd_content)) == -1) {
        perror("write");
        free(*hd_content);
        close(pipe_fd);
        exit(EXIT_FAILURE);
    }

    // Free the heredoc content since it has already been written to the pipe
    free(*hd_content);
    *hd_content = NULL;

    // Close the pipe file descriptor
    close(pipe_fd);
}

void heredoc(Token *token) {
    int pipe_fd[2];
    char *hd_content;

    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Capture the heredoc content
    loop_heredoc(token->heredoc_delimiter, pipe_fd[1], &hd_content);

    close(pipe_fd[1]);  // Close the write-end of the pipe after writing

    // Redirect the read-end of the pipe to stdin
    if (dup2(pipe_fd[0], STDIN_FILENO) == -1) {
        perror("dup2");
        exit(EXIT_FAILURE);
    }

    close(pipe_fd[0]);  // Close the read-end of the pipe since it's now redirected
}











int	unclosed_quotes(char *str)
{
	char	last_opened;

	last_opened = 0;
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
		return (unclosed_quotes(str));
	else if (!last_opened)
		return (0);
	else
		return (1);
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
		while (parsed[i] && !ft_chkchar(QUOTES, parsed[i]))
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

char	*remove_quotes(char	*parsed)
{
	size_t	i;
	size_t	j;
	char	quotes;
	char	*unquoted_parsed;

	i = 0;
	j = 0;
	quotes = '\0';
	unquoted_parsed = malloc((remove_quotes_size(parsed) + 1) * sizeof(char));
	while (parsed[i])
	{
		while (parsed[i] && !ft_chkchar(QUOTES, parsed[i]))
			unquoted_parsed[j++] = parsed[i++];
		if (!parsed[i])
			break ;
		quotes = parsed[i];
		i += 1;
		while (parsed[i] && parsed[i] != quotes)
			unquoted_parsed[j++] = parsed[i++];
		quotes = '\0';
	}
	unquoted_parsed[j] = '\0';
	free(parsed);
	return (unquoted_parsed);
}














// Helper function to get environment variable value
char *get_env_value(const char *var_name, char **envp) {
    size_t len = ft_strlen(var_name);
    for (int i = 0; envp[i]; i++) {
        if (ft_strncmp(envp[i], var_name, len) == 0 && envp[i][len] == '=') {
            return ft_strdup(&envp[i][len + 1]);
        }
    }
    return NULL;
}



// #include "../include/minishell.h"

// Function to get the length of a variable name
size_t get_var_name_length(const char *str, size_t start) {
    size_t length = 0;
    while (str[start + length] && (str[start + length] != ' ' && str[start + length] != '"' && str[start + length] != '\'' && str[start + length] != '$')) {
        length++;
    }
    return length;
}

// Helper function to expand a variable
char *expand_variable(const char *input, size_t *i, char **envp) {
    size_t var_name_length = get_var_name_length(input, *i + 1);
    if (var_name_length == 0) {
        (*i)++;
        return ft_strdup("$");
    }
    
    char *var_name = ft_substr(input, *i + 1, var_name_length);
    char *var_value = get_env_value(var_name, envp);
    free(var_name);
    
    if (!var_value) {
        (*i) += var_name_length + 1;
        return ft_strdup("");
    }
    
    (*i) += var_name_length + 1;
    return var_value;
}

// Function to expand variables in the input string
char *expander(char *input, char **envp) {
    // Check for unclosed quotes
    if (unclosed_quotes(input)) {
        print_error("Syntax error: unclosed quotes", NULL, 1);
        free(input);
        return NULL;
    }

    size_t i = 0, size = 0;
    int in_single_quote = 0;
    int in_double_quote = 0;
    char *expanded_input;

    // First pass: Calculate the size of the expanded string
    while (input[i]) {
        if (input[i] == '\'' && !in_double_quote) {
            in_single_quote = !in_single_quote;
            size++;
            i++;
        } else if (input[i] == '"' && !in_single_quote) {
            in_double_quote = !in_double_quote;
            size++;
            i++;
        } else if (input[i] == '$' && !in_single_quote) {
            if (input[i + 1] == '?') {
                size += ft_strlen(ft_itoa(g_exit));
                i += 2;
            } else {
                char *var_value = expand_variable(input, &i, envp);
                size += ft_strlen(var_value);
                free(var_value);
            }
        } else {
            size++;
            i++;
        }
    }

    // Allocate memory for the expanded string
    expanded_input = malloc((size + 1) * sizeof(char));
    if (!expanded_input) return NULL;
    i = 0;
    size = 0;

    // Second pass: Build the expanded string
    while (input[i]) {
        if (input[i] == '\'' && !in_double_quote) {
            in_single_quote = !in_single_quote;
            expanded_input[size++] = input[i++];
        } else if (input[i] == '"' && !in_single_quote) {
            in_double_quote = !in_double_quote;
            expanded_input[size++] = input[i++];
        } else if (input[i] == '$' && !in_single_quote) {
            if (input[i + 1] == '?') {
                char *exit_status = ft_itoa(g_exit);
                size_t k = 0;
                while (exit_status[k]) expanded_input[size++] = exit_status[k++];
                free(exit_status);
                i += 2;
            } else {
                char *var_value = expand_variable(input, &i, envp);
                size_t k = 0;
                while (var_value[k]) expanded_input[size++] = var_value[k++];
                free(var_value);
            }
        } else {
            expanded_input[size++] = input[i++];
        }
    }
    expanded_input[size] = '\0';

    // Remove quotes from the expanded string
    char *final_expanded_input = remove_quotes(expanded_input);
        if (!final_expanded_input) {
            free(expanded_input);
            return NULL;
        }
    // free(expanded_input);

    // free(input);
    return final_expanded_input;
}



// Tokenizer function to split the input into tokens
// Token *tokenizer(char *input) {
//     Token *head = NULL, *tail = NULL;
//     char **parts = ft_split(input, ' ');
//     char **command = NULL;
//     int command_count = 0;
//     int i = 0;

//     while (parts[i]) {
//         // Remove quotes from the current part
//         parts[i] = remove_quotes(parts[i]);
//             if (!parts[i]) {
//                 // Handle error if needed
//                 free(parts);
//                 return NULL;
//             }

//         if (ft_strcmp(parts[i], "<") == 0) {
//             if (parts[i + 1]) {
//                 if (tail) {
//                     tail->type = TOKEN_REDIRECT_IN;
//                     tail->input_file = ft_strdup(parts[++i]);
//                 } else {
//                     print_error("Redirection operator '<' used without preceding command", NULL, 1);
//                     free(parts);
//                     return NULL;
//                 }
//             } else {
//                 print_error("Expected filename after '<'", NULL, 1);
//                 free(parts);
//                 return NULL;
//             }
//         } else if (ft_strcmp(parts[i], ">") == 0) {
//             if (parts[i + 1]) {
//                 if (tail) {
//                     tail->type = TOKEN_REDIRECT_OUT;
//                     tail->output_file = ft_strdup(parts[++i]);
//                 } else {
//                     print_error("Redirection operator '>' used without preceding command", NULL, 1);
//                     free(parts);
//                     return NULL;
//                 }
//             } else {
//                 print_error("Expected filename after '>'", NULL, 1);
//                 free(parts);
//                 return NULL;
//             }
//         } else if (ft_strcmp(parts[i], "|") == 0) {
//             if (command) {
//                 Token *newToken = new_token(TOKEN_PIPE, command);
//                 if (!newToken) {
//                     free_token_list(head);
//                     free(parts);
//                     return NULL;
//                 }
//                 if (tail) {
//                     tail->next = newToken;
//                 } else {
//                     head = newToken;
//                 }
//                 tail = newToken;
//                 free(command);
//                 command = NULL;
//                 command_count = 0;
//             } else {
//                 print_error("Pipe operator '|' used without preceding command", NULL, 1);
//                 free(parts);
//                 return NULL;
//             }
//         } else {
//             command = realloc(command, (command_count + 2) * sizeof(char *));
//             command[command_count] = ft_strdup(parts[i]);
//             command[command_count + 1] = NULL;
//             command_count++;
//         }
//         i++;
//     }

//     if (command) {
//         Token *newToken = new_token(TOKEN_WORD, command);
//         if (!newToken) {
//             free_token_list(head);
//             free(parts);
//             return NULL;
//         }
//         if (tail) {
//             tail->next = newToken;
//         } else {
//             head = newToken;
//         }
//     }

//     free(command);
//     free(parts);
//     return head;
// }
Token *tokenizer(char *input) {
    Token *head = NULL, *tail = NULL;
    char **parts = ft_split(input, ' ');
    char **command = NULL;
    int command_count = 0;
    int i = 0;

    while (parts[i]) {
        if (ft_strcmp(parts[i], "<<") == 0) {
            if (parts[i + 1]) {
                // Create heredoc token
                Token *newToken = new_token(TOKEN_HEREDOC, NULL);
                if (!newToken) {
                    free_token_list(head);
                    free(parts);
                    return NULL;
                }
                newToken->heredoc_delimiter = ft_strdup(parts[++i]);
                if (!newToken->heredoc_delimiter) {
                    free_token_list(head);
                    free(parts);
                    return NULL;
                }
                if (tail) {
                    tail->next = newToken;
                } else {
                    head = newToken;
                }
                tail = newToken;
            } else {
                print_error("Heredoc '<<' must be followed by a delimiter", NULL, 1);
                free(parts);
                return NULL;
            }
        } else if (ft_strcmp(parts[i], "<") == 0) {
            if (parts[i + 1]) {
                if (tail) {
                    tail->type = TOKEN_REDIRECT_IN;
                    tail->input_file = ft_strdup(parts[++i]);
                    if (!tail->input_file) {
                        free_token_list(head);
                        free(parts);
                        return NULL;
                    }
                } else {
                    print_error("Redirection operator '<' used without preceding command", NULL, 1);
                    free(parts);
                    return NULL;
                }
            } else {
                print_error("Expected filename after '<'", NULL, 1);
                free(parts);
                return NULL;
            }
        } else if (ft_strcmp(parts[i], ">") == 0) {
            if (parts[i + 1]) {
                // Ensure command is properly handled before redirection
                if (command) {
                    Token *newToken = new_token(TOKEN_WORD, command);
                    if (!newToken) {
                        free_token_list(head);
                        free(parts);
                        return NULL;
                    }
                    if (tail) {
                        tail->next = newToken;
                    } else {
                        head = newToken;
                    }
                    tail = newToken;
                    command = NULL;
                    command_count = 0;
                }
                Token *newToken = new_token(TOKEN_REDIRECT_OUT, NULL);
                if (!newToken) {
                    free_token_list(head);
                    free(parts);
                    return NULL;
                }
                newToken->output_file = ft_strdup(parts[++i]);
                if (!newToken->output_file) {
                    free_token_list(head);
                    free(parts);
                    return NULL;
                }
                if (tail) {
                    tail->next = newToken;
                } else {
                    head = newToken;
                }
                tail = newToken;
            } else {
                print_error("Expected filename after '>'", NULL, 1);
                free(parts);
                return NULL;
            }
        } else if (ft_strcmp(parts[i], "|") == 0) {
            if (command) {
                Token *newToken = new_token(TOKEN_WORD, command);
                if (!newToken) {
                    free_token_list(head);
                    free(parts);
                    return NULL;
                }
                if (tail) {
                    tail->next = newToken;
                } else {
                    head = newToken;
                }
                tail = newToken;
                command = NULL;
                command_count = 0;
            } else {
                print_error("Pipe operator '|' used without preceding command", NULL, 1);
                free(parts);
                return NULL;
            }
            // Create a pipe token
            Token *pipeToken = new_token(TOKEN_PIPE, NULL);
            if (!pipeToken) {
                free_token_list(head);
                free(parts);
                return NULL;
            }
            if (tail) {
                tail->next = pipeToken;
            } else {
                head = pipeToken;
            }
            tail = pipeToken;
        } else {
            command = realloc(command, (command_count + 2) * sizeof(char *));
            if (!command) {
                free_token_list(head);
                free(parts);
                return NULL;
            }
            command[command_count] = ft_strdup(parts[i]);
            if (!command[command_count]) {
                free_token_list(head);
                free(parts);
                return NULL;
            }
            command[command_count + 1] = NULL;
            command_count++;
        }
        i++;
    }

    // Handle the last command if any
    if (command) {
        Token *newToken = new_token(TOKEN_WORD, command);
        if (!newToken) {
            free_token_list(head);
            free(parts);
            return NULL;
        }
        if (tail) {
            tail->next = newToken;
        } else {
            head = newToken;
        }
        tail = newToken;
    }

    free(command);
    free(parts);
    return head;
}

// Tokenizer function to split the input into tokens
// Token *tokenizer(char *input) {
//     Token *head = NULL, *tail = NULL;
//     char **parts = ft_split(input, ' ');
//     char **command = NULL;
//     int command_count = 0;
//     int i = 0;

//     while (parts[i]) {
//         if (ft_strcmp(parts[i], "<") == 0) {
//             if (parts[i + 1]) {
//                 if (tail) {
//                     tail->type = TOKEN_REDIRECT_IN;
//                     tail->input_file = ft_strdup(parts[++i]);
//                 } else {
//                     print_error("Redirection operator '<' used without preceding command", NULL, 1);
//                     free(parts);
//                     return NULL;
//                 }
//             } else {
//                 print_error("Expected filename after '<'", NULL, 1);
//                 free(parts);
//                 return NULL;
//             }
//         } else if (ft_strcmp(parts[i], ">") == 0) {
//             if (parts[i + 1]) {
//                 if (tail) {
//                     tail->type = TOKEN_REDIRECT_OUT;
//                     tail->output_file = ft_strdup(parts[++i]);
//                 } else {
//                     print_error("Redirection operator '>' used without preceding command", NULL, 1);
//                     free(parts);
//                     return NULL;
//                 }
//             } else {
//                 print_error("Expected filename after '>'", NULL, 1);
//                 free(parts);
//                 return NULL;
//             }
//         } else if (ft_strcmp(parts[i], "|") == 0) {
//             if (command) {
//                 Token *newToken = new_token(TOKEN_PIPE, command);
//                 if (!newToken) {
//                     free_token_list(head);
//                     free(parts);
//                     return NULL;
//                 }
//                 if (tail) {
//                     tail->next = newToken;
//                 } else {
//                     head = newToken;
//                 }
//                 tail = newToken;
//                 free(command);
//                 command = NULL;
//                 command_count = 0;
//             } else {
//                 print_error("Pipe operator '|' used without preceding command", NULL, 1);
//                 free(parts);
//                 return NULL;
//             }
//         } else {
//             command = realloc(command, (command_count + 2) * sizeof(char *));
//             command[command_count] = ft_strdup(parts[i]);
//             command[command_count + 1] = NULL;
//             command_count++;
//         }
//         i++;
//     }

//     if (command) {
//         Token *newToken = new_token(TOKEN_WORD, command);
//         if (!newToken) {
//             free_token_list(head);
//             free(parts);
//             return NULL;
//         }
//         if (tail) {
//             tail->next = newToken;
//         } else {
//             head = newToken;
//         }
//     }

//     free(command);
//     free(parts);
//     return head;
// }

// Parser function to set redirection files
// void handle_redirection_in(Token *current) {
//     if (current->next && current->next->type == TOKEN_WORD) {
//         current->input_file = ft_strdup(current->next->to_execute[0]);
//         Token *temp = current->next;
//         current->next = current->next->next;
//         free(temp->to_execute);
//         free(temp);
//     } else {
//         print_error("Redirection '<' must be followed by a filename", NULL, 1);
//     }
// }

// void handle_redirection_out(Token *current) {
//     if (current->next && current->next->type == TOKEN_WORD) {
//         current->output_file = ft_strdup(current->next->to_execute[0]);
//         current->append = (current->type == TOKEN_REDIRECT_APPEND);
//         Token *temp = current->next;
//         current->next = current->next->next;
//         free(temp->to_execute);
//         free(temp);
//     } else {
//         print_error("Redirection '>' or '>>' must be followed by a filename", NULL, 1);
//     }
// }

// void handle_heredoc(Token *current) {
//     if (current->next && current->next->type == TOKEN_WORD) {
//         current->heredoc_delimiter = ft_strdup(current->next->to_execute[0]);
//         Token *temp = current->next;
//         current->next = current->next->next;
//         free(temp->to_execute);
//         free(temp);

//         // Process the heredoc by calling the heredoc function
//         heredoc(current);
//     } else {
//         print_error("Heredoc '<<' must be followed by a delimiter", NULL, 1);
//     }
// }
// void parser(Token *head) {
//     Token *current = head;
    
//     while (current) {
//         switch (current->type) {
//             case TOKEN_REDIRECT_IN:
//                 handle_redirection_in(current);
//                 break;
//             case TOKEN_REDIRECT_OUT:
//             case TOKEN_REDIRECT_APPEND:
//                 handle_redirection_out(current);
//                 break;
//             case TOKEN_HEREDOC:
//                 handle_heredoc(current);
//                 break;
//             default:
//                 break;
//         }
//         current = current->next;
//     }
// }
void parser(Token *head) {
    Token *current = head;
    
    while (current) {
        if (current->type == TOKEN_REDIRECT_IN) {
            // Handle input redirection '<'
            if (current->next && current->next->type == TOKEN_WORD) {
                current->input_file = ft_strdup(current->next->to_execute[0]);
                if (!current->input_file) {
                    print_error("Memory allocation error for input file", NULL, 1);
                    return;
                }
                Token *temp = current->next;
                current->next = current->next->next;
                free(temp->to_execute);
                free(temp);
            } else {
                print_error("Redirection '<' must be followed by a filename", NULL, 1);
                return;
            }
        } else if (current->type == TOKEN_REDIRECT_OUT || current->type == TOKEN_REDIRECT_APPEND) {
            // Handle output redirection '>' and '>>'
            if (current->next && current->next->type == TOKEN_WORD) {
                current->output_file = ft_strdup(current->next->to_execute[0]);
                if (!current->output_file) {
                    print_error("Memory allocation error for output file", NULL, 1);
                    return;
                }
                current->append = (current->type == TOKEN_REDIRECT_APPEND);
                Token *temp = current->next;
                current->next = current->next->next;
                free(temp->to_execute);
                free(temp);
            } else {
                print_error("Redirection '>' or '>>' must be followed by a filename", NULL, 1);
                return;
            }
        } else if (current->type == TOKEN_HEREDOC) {
            // Handle heredoc '<<'
            if (current->next && current->next->type == TOKEN_WORD) {
                current->heredoc_delimiter = ft_strdup(current->next->to_execute[0]);
                if (!current->heredoc_delimiter) {
                    print_error("Memory allocation error for heredoc delimiter", NULL, 1);
                    return;
                }
                Token *temp = current->next;
                current->next = current->next->next;
                free(temp->to_execute);
                free(temp);

                // Process the heredoc by calling the heredoc function
                heredoc(current);
            } else {
                print_error("Heredoc '<<' must be followed by a delimiter", NULL, 1);
                return;
            }
        }
        current = current->next;
    }
}

// void parser(Token *head) {
//     Token *current = head;
    
//     while (current) {
//         if (current->type == TOKEN_REDIRECT_IN) {
//             // Handle input redirection '<'
//             if (current->next && current->next->type == TOKEN_WORD) {
//                 current->input_file = ft_strdup(current->next->to_execute[0]);
//                 Token *temp = current->next;
//                 current->next = current->next->next;
//                 free(temp->to_execute);
//                 free(temp);
//             } else {
//                 print_error("Redirection '<' must be followed by a filename", NULL, 1);
//                 return;
//             }
//         } else if (current->type == TOKEN_REDIRECT_OUT || current->type == TOKEN_REDIRECT_APPEND) {
//             // Handle output redirection '>' and '>>'
//             if (current->next && current->next->type == TOKEN_WORD) {
//                 current->output_file = ft_strdup(current->next->to_execute[0]);
//                 current->append = (current->type == TOKEN_REDIRECT_APPEND);
//                 Token *temp = current->next;
//                 current->next = current->next->next;
//                 free(temp->to_execute);
//                 free(temp);
//             } else {
//                 print_error("Redirection '>' or '>>' must be followed by a filename", NULL, 1);
//                 return;
//             }
//         } else if (current->type == TOKEN_HEREDOC) {
//             // Handle heredoc '<<'
//             if (current->next && current->next->type == TOKEN_WORD) {
//                 current->heredoc_delimiter = ft_strdup(current->next->to_execute[0]);
//                 Token *temp = current->next;
//                 current->next = current->next->next;
//                 free(temp->to_execute);
//                 free(temp);

//                 // Process the heredoc by calling the heredoc function
//                 heredoc(current);
//             } else {
//                 print_error("Heredoc '<<' must be followed by a delimiter", NULL, 1);
//                 return;
//             }
//         }
//         current = current->next;
//     }
// }

// Parser function to set redirection files
// void parser(Token *head)
// {
//     Token *current = head;
//     while (current) {
//         if (current->type == TOKEN_REDIRECT_IN) {
//             if (current->next && current->next->type == TOKEN_WORD) {
//                 current->input_file = ft_strdup(current->next->to_execute[0]);
//                 Token *temp = current->next;
//                 current->next = current->next->next;
//                 free(temp->to_execute);
//                 free(temp);
//             } else {
//                 print_error("Redirection '<' must be followed by a filename", NULL, 1);
//                 return;
//             }
//         } else if (current->type == TOKEN_REDIRECT_OUT || current->type == TOKEN_REDIRECT_APPEND) {
//             if (current->next && current->next->type == TOKEN_WORD) {
//                 current->output_file = ft_strdup(current->next->to_execute[0]);
//                 current->append = (current->type == TOKEN_REDIRECT_APPEND);
//                 Token *temp = current->next;
//                 current->next = current->next->next;
//                 free(temp->to_execute);
//                 free(temp);
//             } else {
//                 print_error("Redirection '>' or '>>' must be followed by a filename", NULL, 1);
//                 return;
//             }
//         }
//         current = current->next;
//     }
// }

// Function to check if the input is valid
int is_valid_input(char *input)
{
    if (ft_strlen(input) == 0 ||
        input[0] == '|' ||
        input[ft_strlen(input) - 1] == '|' ||
        input[ft_strlen(input) - 1] == '>' ||
        input[ft_strlen(input) - 1] == '<') {
        return 0;
    }
    return 1;
}

// Function to check if the input contains only spaces
int have_only_spaces(char *input)
{
    while (*input) {
        if (!isspace(*input)) {
            return 0;
        }
        input++;
    }
    return 1;
}

// Function to check redirections validity
int check_redirections(Token *head)
{
    Token *current = head;
    while (current) {
        if ((current->type == TOKEN_REDIRECT_IN || 
             current->type == TOKEN_REDIRECT_OUT || 
             current->type == TOKEN_REDIRECT_APPEND) && 
            !current->input_file && !current->output_file) {
            print_error("Missing file for redirection", NULL, 1);
            return 0;
        }
        current = current->next;
    }
    return 1;
}

// Function to handle commands
void handle_commands(Token *head) {
    Token *current = head;
    while (current) {
        if (current->type == TOKEN_WORD) {
            ft_printf("Executing command: ");
            for (int i = 0; current->to_execute[i]; i++) {
                ft_printf("%s ", current->to_execute[i]);
            }
            ft_printf("\n");
        }
        current = current->next;
    }
}

// Function to print the token list
void print_token_list(Token *head) {
    Token *current = head;
    int i = 1;
    while (current) {
        ft_printf("Command %d\n", i++);
        ft_printf("Token Type: ");
        switch (current->type) {
            case TOKEN_WORD:
                ft_printf("WORD\n");
                break;
            case TOKEN_REDIRECT_IN:
                ft_printf("REDIRECT_IN\n");
                break;
            case TOKEN_REDIRECT_OUT:
                ft_printf("REDIRECT_OUT\n");
                break;
            case TOKEN_REDIRECT_APPEND:
                ft_printf("REDIRECT_APPEND\n");
                break;
            case TOKEN_PIPE:
                ft_printf("PIPE\n");
                break;
            case TOKEN_HEREDOC:
                ft_printf("HEREDOC\n");
                break;
            default:
                ft_printf("UNKNOWN\n");
                break;
        }
        if (current->to_execute) {
            ft_printf("Value: ");
            for (int j = 0; current->to_execute[j]; j++) {
                ft_printf("[%s] ", current->to_execute[j]);
            }
            ft_printf("\n");
        } else {
            ft_printf("Value: NULL\n");
        }
        ft_printf("Input File: %s\n", current->input_file ? current->input_file : "NULL");
        ft_printf("Output File: %s\n", current->output_file ? current->output_file : "NULL");
          ft_printf("Delimiter: %s\n", current->heredoc_delimiter ? current->heredoc_delimiter : "NULL");
                ft_printf("Content:%s\n", current->heredoc_content ? current->heredoc_content : "NULL");
        ft_printf("Append: %d\n", current->append);
        ft_printf("Pipe to Next Token: %d\n", current->pipe_to_next_token);
        ft_printf("\n");
        current = current->next;
    }
}

// Function to prompt for user input
char *ft_prompt()
{
	char *v_return;

	v_return = readline("minishell>: ");
	if (ft_strlen(v_return) == 0)
		return (free(v_return), printf("\n"), ft_prompt());
	add_history(v_return);
	return (v_return);
}

// Function to process the input
int process_input(char *input)
{
    if (!input) {
        ft_printf("Exit\n");
        exit(g_exit);
    }
    if (have_only_spaces(input)) {
        free(input);
        return 1; 
    }
    if (!is_valid_input(input)) {
        print_error("Invalid input format", NULL, 1);
        free(input);
        return 1; 
    }
    // if (!check_quotes(input)) {
    //     print_error("Mismatched quotes", NULL, 1);
    //     free(input);
    //     return 1;
    // }
    return 0;
}

void sig_handler(int signum)
{
    if (signum == SIGINT) {
        rl_replace_line("", 0);
        write(1, "\n", 1);
        rl_on_new_line();
        rl_redisplay();
        g_exit = 130;
    }
}

