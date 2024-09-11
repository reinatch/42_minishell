#include "../include/minishell.h"
// Global exit status
int g_exit = 0;

// Function to free the token list
void free_command_list(t_command *head)
{
    t_command *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        if (temp->to_execute) {
            int i = 0;
            while (temp->to_execute[i] != NULL) {
                free(temp->to_execute[i]);
                i++;
            }
            free(temp->to_execute);
        }
        if (temp->input_file) {
            free(temp->input_file);
        }
        if (temp->output_file) {
            free(temp->output_file);
        }
        if (temp->heredoc_delimiter) {
            free(temp->heredoc_delimiter);
        }
        if (temp->heredoc_content) {
            free(temp->heredoc_content);
        }
        // Free the t_command struct itself
        free(temp);
    }
}

// Function to find the next quote in the string
int find_next_quote(char *line, int start, int *count, char quote)
{
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
int check_quotes(char *line)
{
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
void print_error(char *msg, char *key, int exit_code)
{
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
int ft_chkchar(const char *str, int ch)
{
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

// Function to get the length of a variable name
size_t get_var_name_length(const char *str, size_t start) {
    size_t length = 0;
    while (str[start + length] && (str[start + length] != ' ' && str[start + length] != '"' && str[start + length] != '\'' && str[start + length] != '$')) {
        length++;
    }
    return length;
}

// Helper function to expand a variable
char *expand_variable(const char *input, size_t *i, char **envp)
{
    size_t var_name_length;
    char *var_name;
    char *var_value;

    var_name_length = get_var_name_length(input, *i + 1);
    if (var_name_length == 0) {
        (*i)++;
        return ft_strdup("$");
    }
    var_name = ft_substr(input, *i + 1, var_name_length);
    var_value = get_env_value(var_name, envp);
    free(var_name);
    if (!var_value) {
        (*i) += var_name_length + 1;
        return ft_strdup("");
    }
    (*i) += var_name_length + 1;
    return var_value;
}

// Helper function to free the split array
void ft_free_split(char **split) {
    if (!split) return;
    for (int i = 0; split[i]; i++) {
        free(split[i]);
    }
    free(split);
}

// Helper function to expand variables in a single line
char *expand_line(char *line, t_program *program) {
    size_t i = 0;
    size_t size = 0;
    char *expanded_line;
    int in_single_quote = 0;
    int in_double_quote = 0;

    // First pass: calculate the size of the expanded string
    while (line[i]) {
        if (line[i] == '\'' && !in_double_quote) {
            in_single_quote = !in_single_quote;
            size++;
            i++;
        } else if (line[i] == '"' && !in_single_quote) {
            in_double_quote = !in_double_quote;
            size++;
            i++;
        } else if (line[i] == '$' && !in_single_quote) {
            if (line[i + 1] == '?') {
                size += ft_strlen(ft_itoa(g_exit));
                i += 2;
            } else {
                char *var_value = expand_variable(line, &i, program->my_env);
                size += ft_strlen(var_value);
                free(var_value);
            }
        } else {
            size++;
            i++;
        }
    }

    // Allocate memory for the expanded line
    expanded_line = malloc((size + 1) * sizeof(char));
    if (!expanded_line) return NULL;

    // Reset variables for second pass
    i = 0;
    size = 0;

    // Second pass: build the expanded string
    while (line[i]) {
        if (line[i] == '\'' && !in_double_quote) {
            in_single_quote = !in_single_quote;
            expanded_line[size++] = line[i++];
        } else if (line[i] == '"' && !in_single_quote) {
            in_double_quote = !in_double_quote;
            expanded_line[size++] = line[i++];
        } else if (line[i] == '$' && !in_single_quote) {
            if (line[i + 1] == '?') {
                char *exit_status = ft_itoa(g_exit);
                size_t k = 0;
                while (exit_status[k]) expanded_line[size++] = exit_status[k++];
                free(exit_status);
                i += 2;
            } else {
                char *var_value = expand_variable(line, &i, program->my_env);
                size_t k = 0;
                while (var_value[k]) expanded_line[size++] = var_value[k++];
                free(var_value);
            }
        } else {
            expanded_line[size++] = line[i++];
        }
    }
    expanded_line[size] = '\0';

    return expanded_line;
}

// Function to expand variables in the input string (supports multi-line)
char *expander(char *input, t_program *program) {
    char **lines;
    char *expanded_input = ft_strdup("");
    char *temp;
    size_t i = 0;

    lines = ft_split(input, '\n');
    
    while (lines[i]) {
        char *expanded_line = expand_line(lines[i], program);
        temp = expanded_input;
        expanded_input = ft_strjoin(expanded_input, expanded_line);
        expanded_input = ft_strjoin(expanded_input, "\n");

        free(temp);
        free(expanded_line);
        i++;
    }
    ft_free_split(lines);
    return expanded_input;
}

char *ft_strdup(const char *s1) {
    char *copy = malloc(strlen(s1) + 1);
    if (copy)
        strcpy(copy, s1);
    return copy;
}
t_lexer *new_token(char *src, t_tokenType type) 
{
    t_lexer *new = malloc(sizeof(t_lexer));
    new->str = ft_strdup(src);
    new->size = strlen(src);
    new->type = type;
    new->next = NULL;
    return new;
}

void add_token(t_lexer **head, t_lexer *new_token) 
{
    if (*head == NULL) {
        *head = new_token;
    } else {
        t_lexer *temp = *head;
        while (temp->next)
            temp = temp->next;
        temp->next = new_token;
    }
}

void free_token_list(t_lexer *head) 
{
    t_lexer *temp;
    while (head) {
        temp = head;
        head = head->next;
        free(temp->str);
        free(temp);
    }
}

// void free_command_list(t_command *head) {
//     t_command *temp;
//     while (head) {
//         temp = head;
//         head = head->next;
//         if (temp->to_execute) {
//             for (int i = 0; temp->to_execute[i]; i++)
//                 free(temp->to_execute[i]);
//             free(temp->to_execute);
//         }
//         free(temp->input_file);
//         free(temp->output_file);
//         free(temp->heredoc_delimiter);
//         free(temp->heredoc_content);
//         free(temp);
//     }
// }

t_lexer *tokenizer(char *input) {
    t_lexer *lexer = NULL;
    char *start;
    while (*input) {
        if (isspace(*input)) {
            input++;
            continue;
        }
        if (strncmp(input, "<<", 2) == 0) {
            add_token(&lexer, new_token("<<", TOKEN_HEREDOC));
            input += 2;
        } else if (strncmp(input, ">>", 2) == 0) {
            add_token(&lexer, new_token(">>", TOKEN_REDIRECT_APPEND));
            input += 2;
        } else if (*input == '<') {
            add_token(&lexer, new_token("<", TOKEN_REDIRECT_IN));
            input++;
        } else if (*input == '>') {
            add_token(&lexer, new_token(">", TOKEN_REDIRECT_OUT));
            input++;
        } else if (*input == '|') {
            add_token(&lexer, new_token("|", TOKEN_PIPE));
            input++;
        } else {
            start = input;
            while (*input && !isspace(*input) && *input != '<' && *input != '>' && *input != '|')
                input++;
            add_token(&lexer, new_token(strndup(start, input - start), TOKEN_WORD));
        }
    }
    return lexer;
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

int count_args(t_lexer *lexer) {
    int count = 0;
    while (lexer && lexer->type != TOKEN_PIPE) {
        if (lexer->type == TOKEN_WORD)
            count++;
        lexer = lexer->next;
    }
    return count;
}

void add_command(t_command **head, t_command *new_command) {
    if (*head == NULL) {
        *head = new_command;
    } else {
        t_command *temp = *head;
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = new_command;
    }
}

// Function to initialize a new command
t_command *new_command() 
{
    t_command *cmd = (t_command *)malloc(sizeof(t_command));
    
    if (!cmd) return NULL; 
    cmd->to_execute = NULL;
    cmd->input_file = NULL;
    cmd->output_file = NULL;
    cmd->append = 0;
    cmd->heredoc_delimiter = NULL;
    cmd->heredoc_content = NULL;
    cmd->pipe_to_next_token = 0;
    cmd->next = NULL;
    return cmd;
}
void loop_heredoc(char *delimiter, char **hd_content) 
{
    char *line;
    char *new_content;
    *hd_content = ft_strdup("");
    char *old_content; 
    while (1) {
        line = readline("> ");
        if (!line || !ft_strncmp(line, delimiter, ft_strlen(delimiter) + 1)) 
        {
            if (!line) {
                perror("Error reading heredoc input");
                exit(EXIT_FAILURE);
            }
            free(line);
            break;
        }
        // Concatenate each line into heredoc content
        new_content = ft_strjoin(line, "\n");
        old_content = *hd_content;
        *hd_content = ft_strjoin(old_content, new_content);
        free(old_content);
        free(new_content);
        free(line);
    }
}

void heredoc(t_program *program) {
    char *hd_content;
    char *expanded;

    // Capture the entire heredoc content
    loop_heredoc(program->commands->heredoc_delimiter, &hd_content);

    // Expand variables in the heredoc content
    expanded = expander(hd_content, program);

    if (expanded) {
        // Store expanded heredoc content in the command
        program->commands->heredoc_content = ft_strdup(expanded);
    } else {
        // If expansion failed, store the original content
        program->commands->heredoc_content = ft_strdup(hd_content);
    }

    // Free memory
    free(hd_content);
    free(expanded);
}




t_program *parser(t_program *program)
{

    t_command *head = NULL;
    t_command *current_cmd = NULL;

    while (program->tokens) {
        if (program->tokens->type == TOKEN_WORD) {
            current_cmd = malloc(sizeof(t_command));
            if (!current_cmd) {
                fprintf(stderr, "Memory allocation failed\n");
                free_command_list(head);
                exit(EXIT_FAILURE);
            }
            memset(current_cmd, 0, sizeof(t_command));
            add_command(&head, current_cmd);
            int argc = count_args(program->tokens);
            current_cmd->to_execute = calloc(argc + 1, sizeof(char *));
            if (!current_cmd->to_execute) {
                fprintf(stderr, "Memory allocation failed\n");
                free_command_list(head);
                exit(EXIT_FAILURE);
            }
            int arg_index = 0;
            while (program->tokens && program->tokens->type != TOKEN_PIPE) {
                if (program->tokens->type == TOKEN_WORD) {
                    current_cmd->to_execute[arg_index++] = ft_strdup(program->tokens->str);
                } else if (program->tokens->type == TOKEN_REDIRECT_IN) {
                    program->tokens = program->tokens->next;
                    if (program->tokens && program->tokens->type == TOKEN_WORD) {
                        current_cmd->input_file = ft_strdup(program->tokens->str);
                    }
                } else if (program->tokens->type == TOKEN_REDIRECT_OUT) {
                    program->tokens = program->tokens->next;
                    if (program->tokens && program->tokens->type == TOKEN_WORD) {
                        current_cmd->output_file = ft_strdup(program->tokens->str);
                    }
                } else if (program->tokens->type == TOKEN_REDIRECT_APPEND) {
                    program->tokens = program->tokens->next;
                    if (program->tokens && program->tokens->type == TOKEN_WORD) {
                        current_cmd->output_file = ft_strdup(program->tokens->str);
                        current_cmd->append = 1;
                    }
                } else if (program->tokens->type == TOKEN_HEREDOC) {
                    program->tokens = program->tokens->next;
                    if (program->tokens && program->tokens->type == TOKEN_WORD) {
                        current_cmd->heredoc_delimiter = ft_strdup(program->tokens->str);
                        current_cmd->heredoc_content = NULL;
                    }
                }
                program->tokens = program->tokens->next;
            }
            if (program->tokens && program->tokens->type == TOKEN_PIPE) {
                current_cmd->pipe_to_next_token = 1;
                program->tokens = program->tokens->next;
                current_cmd = NULL;
            }
        } else {
            program->tokens = program->tokens->next;
        }
    }
    t_command *cmd = head;
    program->commands = head;
    while (cmd) {
        if (cmd->heredoc_delimiter) {
            heredoc(program);
        }
        cmd = cmd->next;
    }
    return program;
}


int check_redirections(t_command *head)
{
    t_command *current = head;
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

void print_command_list(t_command *current) {
    int i = 1;
    while (current) {
        printf("Command %d\n", i++);
        printf("Token Type: ");
        if (current->type == TOKEN_WORD) {
            printf("WORD\n");
        } else if (current->type == TOKEN_REDIRECT_IN) {
            printf("REDIRECT_IN\n");
        } else if (current->type == TOKEN_REDIRECT_OUT) {
            printf("REDIRECT_OUT\n");
        } else if (current->type == TOKEN_REDIRECT_APPEND) {
            printf("REDIRECT_APPEND\n");
        } else if (current->type == TOKEN_PIPE) {
            printf("PIPE\n");
        } else if (current->type == TOKEN_HEREDOC) {
            printf("HEREDOC\n");
        } else {
            printf("UNKNOWN\n");
        }

        if (current->to_execute) {
            printf("Value: ");
            for (int j = 0; current->to_execute[j]; j++) {
                printf("[%s] ", current->to_execute[j]);
            }
            printf("\n");
        } else {
            printf("Value: NULL\n");
        }

        printf("Input File: %s\n", current->input_file ? current->input_file : "NULL");
        printf("Output File: %s\n", current->output_file ? current->output_file : "NULL");
        printf("Delimiter: %s\n", current->heredoc_delimiter ? current->heredoc_delimiter : "NULL");
        printf("Content: %s\n", current->heredoc_content ? current->heredoc_content : "NULL");
        printf("Append: %d\n", current->append);
        printf("Pipe to Next Token: %d\n", current->pipe_to_next_token);
        printf("\n");

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
