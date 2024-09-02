

#include "../include/minishell.h"


// Global exit status
int g_exit = 0;



// Function to create a new token
Token *new_token(TokenType type, char **value) {
    Token *token = (Token *)malloc(sizeof(Token));
    if (!token) return NULL;

    token->type = type;

    // Calculate the size of the value array
    size_t count = 0;
    while (value[count]) {
        count++;
    }

    // Allocate memory for to_execute array
    token->to_execute = (char **)malloc((count + 1) * sizeof(char *));
    if (!token->to_execute) {
        free(token);
        return NULL;
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

    token->input_file = NULL;
    token->output_file = NULL;
    token->append = 0;
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
int find_next_quote(char *line, int start, int *count, char quote)
{
    int i = start + 1;
    *count += 1;
    while (line[i] && line[i] != quote)
        i++;
    if (line[i] == quote)
        *count += 1;
    return (i - start);
}

// Function to check for balanced quotes
int check_quotes(char *line)
{
    int i = 0;
    int double_quote_count = 0;
    int single_quote_count = 0;
    while (line[i]) {
        if (line[i] == '\'')
            i += find_next_quote(line, i, &single_quote_count, '\'');
        else if (line[i] == '"')
            i += find_next_quote(line, i, &double_quote_count, '"');
        else
            i++;
    }
    return (double_quote_count % 2 == 0) && (single_quote_count % 2 == 0);
}

// Function to print error messages
void print_error(char *msg, char *key, int exit_code)
{
    if (key) {
        if (msg)
            ft_printf("%s: %s: %s\n", ERROR_TITLE, key, msg);
        else
            ft_printf("%s: %s `%s'\n", ERROR_TITLE, ERROR_SYNTAX, key);
    } else {
        ft_printf("%s: %s\n", ERROR_TITLE, msg);
    }
    g_exit = exit_code;
}

// Tokenizer function to split the input into tokens
Token *tokenizer(char *input)
{
    Token *head = NULL, *tail = NULL;
    char **parts = ft_split(input, ' ');
    char **command = NULL;
    int command_count = 0;
    int i = 0;

    while (parts[i]) {
        if (ft_strcmp(parts[i], "<") == 0) {
            if (parts[i + 1]) {
                if (tail) {
                    tail->type = TOKEN_REDIRECT_IN;
                    tail->input_file = ft_strdup(parts[++i]);
                } else {
                    print_error("Redirection operator '<' used without preceding command", NULL, 1);
                    free(parts);
                    return NULL;
                }
            } else {
                print_error("Redirection operator '<' missing filename", NULL, 1);
                free(parts);
                return NULL;
            }
        } else if (ft_strcmp(parts[i], ">") == 0) {
            if (parts[i + 1]) {
                if (tail) {
                    tail->type = TOKEN_REDIRECT_OUT;
                    tail->output_file = ft_strdup(parts[++i]);
                    tail->append = 0;
                } else {
                    print_error("Redirection operator '>' used without preceding command", NULL, 1);
                    free(parts);
                    return NULL;
                }
            } else {
                print_error("Redirection operator '>' missing filename", NULL, 1);
                free(parts);
                return NULL;
            }
        } else if (ft_strcmp(parts[i], ">>") == 0) {
            if (parts[i + 1]) {
                if (tail) {
                    tail->type = TOKEN_REDIRECT_APPEND;
                    tail->output_file = ft_strdup(parts[++i]);
                    tail->append = 1;
                } else {
                    print_error("Redirection operator '>>' used without preceding command", NULL, 1);
                    free(parts);
                    return NULL;
                }
            } else {
                print_error("Redirection operator '>>' missing filename", NULL, 1);
                free(parts);
                return NULL;
            }
        } else if (ft_strcmp(parts[i], "|") == 0) {
            if (tail) {
                tail->pipe_to_next_token = 1;
            } else {
                print_error("Pipe operator '|' used without preceding command", NULL, 1);
                free(parts);
                return NULL;
            }
        } else {
            // Handle command and arguments
            command_count++;
            command = realloc(command, sizeof(char *) * (command_count + 1));
            command[command_count - 1] = ft_strdup(parts[i]);
            command[command_count] = NULL;

            if (command && (!parts[i + 1] || ft_strcmp(parts[i + 1], "|") == 0 || ft_strcmp(parts[i + 1], "<") == 0 || ft_strcmp(parts[i + 1], ">") == 0 || ft_strcmp(parts[i + 1], ">>") == 0)) {
                Token *new_tok = new_token(TOKEN_WORD, command);
                if (!head) {
                    head = new_tok;
                } else {
                    tail->next = new_tok;
                }
                tail = new_tok;
                command = NULL;
                command_count = 0;
            }
        }
        i++;
    }

    free(parts);
    return head;
}

// Parser function to set redirection files
void parser(Token *head)
{
    Token *current = head;
    while (current) {
        if (current->type == TOKEN_REDIRECT_IN) {
            if (current->next && current->next->type == TOKEN_WORD) {
                current->input_file = ft_strdup(current->next->to_execute[0]);
                Token *temp = current->next;
                current->next = current->next->next;
                free(temp->to_execute);
                free(temp);
            } else {
                print_error("Redirection '<' must be followed by a filename", NULL, 1);
                return;
            }
        } else if (current->type == TOKEN_REDIRECT_OUT || current->type == TOKEN_REDIRECT_APPEND) {
            if (current->next && current->next->type == TOKEN_WORD) {
                current->output_file = ft_strdup(current->next->to_execute[0]);
                current->append = (current->type == TOKEN_REDIRECT_APPEND);
                Token *temp = current->next;
                current->next = current->next->next;
                free(temp->to_execute);
                free(temp);
            } else {
                print_error("Redirection '>' or '>>' must be followed by a filename", NULL, 1);
                return;
            }
        }
        current = current->next;
    }
}

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
        ft_printf("command %d\n", i);
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
            case TOKEN_EOF:
                ft_printf("EOF\n");
                break;
            default:
                ft_printf("UNKNOWN\n");
                break;
        }
        if (current->to_execute) {
            ft_printf("Value: ");
            for (int j = 0; current->to_execute[j]; j++) {
                ft_printf("[%s], ", current->to_execute[j]);
            }
            ft_printf("\n");
        } else {
            ft_printf("Value: NULL\n");
        }
        ft_printf("Input File: %s\n", current->input_file ? current->input_file : "NULL");
        ft_printf("Output File: %s\n", current->output_file ? current->output_file : "NULL");
        ft_printf("Append: %d\n", current->append);
        ft_printf("Pipe to Next Token: %d\n", current->pipe_to_next_token);
        ft_printf("\n");
        current = current->next;
        i++;
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
    if (!check_quotes(input)) {
        print_error("Mismatched quotes", NULL, 1);
        free(input);
        return 1;
    }
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







































































