#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>


// Explanation and Highlights

//     Token Structure: Token structure is defined with type, value, and next pointer to create a linked list of tokens.

//     Lexer (read_input and get_next_token):
//         read_input() reads a line of input using readline.
//         get_next_token(input) parses the input into tokens (TOKEN_WORD, redirections, pipes, etc.) and returns a linked list of tokens.

//     Parser (parse_command):
//         parse_command(tokens) parses tokens into a Command structure (args for arguments, input_file, output_file, append_output, pipe_to_next).

//     Executor (execute_command):
//         execute_command(cmd) forks a child process to execute the command using execvp.
//         Handles redirection of input and output based on parsed command.

//     Signal Handling (handle_signal):
//         handle_signal(signal) handles SIGINT and SIGQUIT signals using volatile sig_atomic_t.

//     Main Function:
//         Sets up signal handling (sigaction).
//         Loops to read input, tokenize, parse, expand variables, execute commands, and handle memory cleanup.

// Compilation

// To compile this example code, use the following command (assuming you have the readline library installed):

// sh

// gcc mini_shell.c -o myshell -lreadline





// Define token types
typedef enum {
    TOKEN_WORD,
    TOKEN_REDIRECT_IN,
    TOKEN_REDIRECT_OUT,
    TOKEN_REDIRECT_APPEND,
    TOKEN_PIPE,
    TOKEN_EOF
} TokenType;

// Define token structure with linked list
typedef struct Token {
    TokenType type;
    char *value;
    struct Token *next;
} Token;

volatile sig_atomic_t signal_received = 0;

// Signal handler function
void handle_signal(int signal) {
    signal_received = signal;
}

// Function for reading input (Lexer)
char *read_input() {
    char *input = readline("myshell> ");
    if (input && *input) {
        add_history(input);
    }
    return input;
}

// Function for getting next token (Lexer)
Token *get_next_token(char *input) {
    static char *current_pos = NULL;
    char *token_str;

    // Initialize on first call
    if (input != NULL) {
        current_pos = input;
    }

    // Skip leading whitespace
    while (*current_pos == ' ' || *current_pos == '\t') {
        current_pos++;
    }

    // Check for end of input
    if (*current_pos == '\0') {
        Token *token = malloc(sizeof(Token));
        token->type = TOKEN_EOF;
        token->value = NULL;
        token->next = NULL;
        return token;
    }

    // Handle redirections, pipes, and words
    if (*current_pos == '<') {
        token_str = strndup(current_pos, 1);
        current_pos++;
        Token *token = malloc(sizeof(Token));
        token->type = TOKEN_REDIRECT_IN;
        token->value = token_str;
        token->next = get_next_token(NULL); // Recursively get next token
        return token;
    } else if (*current_pos == '>') {
        if (*(current_pos + 1) == '>') {
            token_str = strndup(current_pos, 2);
            current_pos += 2;
            Token *token = malloc(sizeof(Token));
            token->type = TOKEN_REDIRECT_APPEND;
            token->value = token_str;
            token->next = get_next_token(NULL); // Recursively get next token
            return token;
        } else {
            token_str = strndup(current_pos, 1);
            current_pos++;
            Token *token = malloc(sizeof(Token));
            token->type = TOKEN_REDIRECT_OUT;
            token->value = token_str;
            token->next = get_next_token(NULL); // Recursively get next token
            return token;
        }
    } else if (*current_pos == '|') {
        token_str = strndup(current_pos, 1);
        current_pos++;
        Token *token = malloc(sizeof(Token));
        token->type = TOKEN_PIPE;
        token->value = token_str;
        token->next = get_next_token(NULL); // Recursively get next token
        return token;
    } else if (*current_pos == '\'' || *current_pos == '"') {
        char quote = *current_pos;
        current_pos++;
        char *start = current_pos;
        while (*current_pos != '\0' && *current_pos != quote) {
            current_pos++;
        }
        token_str = strndup(start, current_pos - start);
        if (*current_pos == quote) {
            current_pos++;
        }
        Token *token = malloc(sizeof(Token));
        token->type = TOKEN_WORD;
        token->value = token_str;
        token->next = get_next_token(NULL); // Recursively get next token
        return token;
    } else {
        char *start = current_pos;
        while (*current_pos != '\0' && *current_pos != ' ' && *current_pos != '\t' &&
               *current_pos != '<' && *current_pos != '>' && *current_pos != '|' &&
               *current_pos != '\'' && *current_pos != '"') {
            current_pos++;
        }
        token_str = strndup(start, current_pos - start);
        Token *token = malloc(sizeof(Token));
        token->type = TOKEN_WORD;
        token->value = token_str;
        token->next = get_next_token(NULL); // Recursively get next token
        return token;
    }
}

// Function for freeing tokens (Lexer)
void free_token(Token *token) {
    if (token != NULL) {
        free(token->value);
        free_token(token->next); // Recursively free linked tokens
        free(token);
    }
}

// Structure for parsed command (Parser)
typedef struct {
    char **args;
    char *input_file;
    char *output_file;
    int append_output;
    int pipe_to_next;
} Command;

// Function for parsing command (Parser)
Command *parse_command(Token **tokens) {
    Command *cmd = malloc(sizeof(Command));
    cmd->args = malloc(100 * sizeof(char*)); // Assuming a maximum of 100 arguments

    int i = 0;
    while ((*tokens)->type == TOKEN_WORD || (*tokens)->type == TOKEN_REDIRECT_IN ||
           (*tokens)->type == TOKEN_REDIRECT_OUT || (*tokens)->type == TOKEN_REDIRECT_APPEND) {
        if ((*tokens)->type == TOKEN_WORD) {
            cmd->args[i++] = (*tokens)->value;
        } else if ((*tokens)->type == TOKEN_REDIRECT_IN) {
            (*tokens) = (*tokens)->next; // Move to next token after redirect symbol
            cmd->input_file = (*tokens)->value;
        } else if ((*tokens)->type == TOKEN_REDIRECT_OUT) {
            (*tokens) = (*tokens)->next; // Move to next token after redirect symbol
            cmd->output_file = (*tokens)->value;
        } else if ((*tokens)->type == TOKEN_REDIRECT_APPEND) {
            cmd->append_output = 1;
            (*tokens) = (*tokens)->next; // Move to next token after redirect symbol
            cmd->output_file = (*tokens)->value;
        }
        (*tokens) = (*tokens)->next;
    }
    cmd->args[i] = NULL; // NULL terminate arguments list

    if ((*tokens)->type == TOKEN_PIPE) {
        cmd->pipe_to_next = 1;
        (*tokens) = (*tokens)->next; // Move to next token after pipe symbol
    } else {
        cmd->pipe_to_next = 0;
    }

    return cmd;
}

// Function for expanding variables (Expander)
char *expand_variables(char *arg) {
    // Example: expand $VAR to its value
    // For simplicity, no expansion in this example
    return strdup(arg); // No expansion needed
}

// Function for executing command (Executor)
int execute_command(Command *cmd) {
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        if (cmd->input_file != NULL) {
            freopen(cmd->input_file, "r", stdin);
        }
        if (cmd->output_file != NULL) {
            if (cmd->append_output) {
                freopen(cmd->output_file, "a", stdout);
            } else {
                freopen(cmd->output_file, "w", stdout);
            }
        }
        execvp(cmd->args[0], cmd->args);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Fork failed
        perror("fork");
        return 0;
    } else {
        // Parent process
		waitpid(pid, &status, 0);
		return 1;
    }
}

// Main function
int main() {
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, NULL);  // Handle Ctrl-C
    sigaction(SIGQUIT, &sa, NULL); // Handle Ctrl-\ ,

    char *input;
    Token *token;
    Command *cmd;
    int status = 1;

    do {
        input = read_input();
        if (input == NULL) {
            break;
        }

        token = get_next_token(input);
        while (token->type != TOKEN_EOF) {
            cmd = parse_command(&token);

            // Expander (for each argument in cmd->args)
            for (int i = 0; cmd->args[i] != NULL; i++) {
                cmd->args[i] = expand_variables(cmd->args[i]);
            }

            // Executor
            status = execute_command(cmd);

            // Free command structure
            free(cmd->args);
            free(cmd);

            // Move to next token
            free_token(token);
            token = get_next_token(NULL);
        }

        free(input);
    } while (status);

    return 0;
}
