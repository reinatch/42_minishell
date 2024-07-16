
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

# include "./includes/libft.h"

# include <readline/readline.h>
# include <readline/history.h>

# include <sys/types.h>
# include <dirent.h>
# include <signal.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <errno.h>


// Define the token types
typedef enum {
    TOKEN_WORD,
    TOKEN_REDIRECT_IN,
    TOKEN_REDIRECT_OUT,
    TOKEN_REDIRECT_APPEND,
    TOKEN_PIPE,
    TOKEN_EOF
} TokenType;

// Define the Token structure
typedef struct Token {
    TokenType type;
    char *value;
    char *input_file;
    char *output_file;
    int append;
    int pipe_to_next_token;
    struct Token *next;
} Token;

// Function to create a new token
Token *new_token(TokenType type, char *value) {
    Token *token = (Token *)malloc(sizeof(Token));
    if (!token) return NULL;
    token->type = type;
    token->value = value ? strdup(value) : NULL;
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
        free(temp->value);
        free(temp->input_file);
        free(temp->output_file);
        free(temp);
    }
}
Token *tokenizer(char *input) {
    Token *head = NULL, *tail = NULL;
    char **parts = ft_split(input, ' ');

    TokenType type = TOKEN_WORD;
    char *command = NULL;
    char *redirection_file = NULL;
    int is_append = 0; // Flag to check if we encounter >>
    for (int i = 0; parts[i]; i++) {
        if (strcmp(parts[i], "<") == 0) {
            // Create token for current command before the redirection
            if (command) {
                Token *new_tok = new_token(type, command);
                if (!head) {
                    head = new_tok;
                } else {
                    tail->next = new_tok;
                }
                tail = new_tok;

                // Set input_file for the last command
                tail->input_file = redirection_file ? strdup(redirection_file) : NULL;
                redirection_file = NULL; // Reset redirection file
                command = NULL;
            }

            // Set type to TOKEN_REDIRECT_IN and continue
            type = TOKEN_REDIRECT_IN;
            continue;
        } else if (strcmp(parts[i], ">") == 0 || strcmp(parts[i], ">>") == 0) {
            // Create token for current command before the redirection
            if (command) {
                Token *new_tok = new_token(type, command);
                if (!head) {
                    head = new_tok;
                } else {
                    tail->next = new_tok;
                }
                tail = new_tok;

                // Set output_file for the last command
                tail->output_file = redirection_file ? strdup(redirection_file) : NULL;
                tail->append = (strcmp(parts[i], ">>") == 0); // Set append flag for >>
                redirection_file = NULL; // Reset redirection file
                command = NULL;
            }

            // Set type to TOKEN_REDIRECT_OUT and continue
            type = TOKEN_REDIRECT_OUT;
            continue;
        } else if (strcmp(parts[i], "|") == 0) {
            // Create token for current command before the pipe
            if (command) {
                Token *new_tok = new_token(type, command);
                if (!head) {
                    head = new_tok;
                } else {
                    tail->next = new_tok;
                }
                tail = new_tok;

                // Set pipe_to_next_token for the last command
                tail->pipe_to_next_token = 1;

                command = NULL;
            }

            // Set type to TOKEN_PIPE and continue
            type = TOKEN_PIPE;
            continue;
        }

        // For regular words (arguments or command parts)
        if (!command) {
            command = strdup(parts[i]);
        } else {
            size_t len = strlen(command) + strlen(parts[i]) + 2;
            command = realloc(command, len);
            strcat(command, " ");
            strcat(command, parts[i]);
        }
    }

    // Create token for the last command or argument
    if (command) {
        Token *new_tok = new_token(type, command);
        if (!head) {
            head = new_tok;
        } else {
            tail->next = new_tok;
        }
        tail = new_tok;

        // Set input_file or output_file for the last command based on the last redirection_file
        if (type == TOKEN_REDIRECT_IN || type == TOKEN_REDIRECT_OUT || type == TOKEN_REDIRECT_APPEND) {
            tail->input_file = redirection_file ? strdup(redirection_file) : NULL;
        } else {
            tail->output_file = redirection_file ? strdup(redirection_file) : NULL;
            tail->append = is_append;
        }
    }

    free(parts);
    return head;
}








void parser(Token *head) {
    Token *current = head;
    while (current) {
        if (current->type == TOKEN_REDIRECT_IN && current->next) {
            current->input_file = strdup(current->next->value);
            Token *temp = current->next;
            current->next = current->next->next;
            free(temp->value);
            free(temp);
        } else if ((current->type == TOKEN_REDIRECT_OUT || current->type == TOKEN_REDIRECT_APPEND) && current->next) {
            current->output_file = strdup(current->next->value);
            current->append = (current->type == TOKEN_REDIRECT_APPEND);
            Token *temp = current->next;
            current->next = current->next->next;
            free(temp->value);
            free(temp);
        }
        current = current->next;
    }
}

void print_token_list(Token *head) {
    Token *current = head;
    while (current) {
        printf("Token Type: ");
        switch (current->type) {
            case TOKEN_WORD:
                printf("WORD\n");
                break;
            case TOKEN_REDIRECT_IN:
                printf("REDIRECT_IN\n");
                break;
            case TOKEN_REDIRECT_OUT:
                printf("REDIRECT_OUT\n");
                break;
            case TOKEN_REDIRECT_APPEND:
                printf("REDIRECT_APPEND\n");
                break;
            case TOKEN_EOF:
                printf("EOF\n");
                break;
            default:
                printf("UNKNOWN\n");
                break;
        }
        printf("Value: %s\n", current->value ? current->value : "NULL");
        printf("Input File: %s\n", current->input_file ? current->input_file : "NULL");
        printf("Output File: %s\n", current->output_file ? current->output_file : "NULL");
        printf("Append: %d\n", current->append);
        printf("Pipe to Next Token: %d\n", current->pipe_to_next_token);
        printf("\n");
        current = current->next;
    }
}

char *ft_prompt() {
    char *v_return;

    v_return = readline("minishell>: ");
    if (strlen(v_return) == 0) {
        free(v_return);
        printf("\n");
        return ft_prompt();
    }
    add_history(v_return);
    return v_return;
}

int main() {
    char *input;
    Token *cmds;
    while (1) {
        input = ft_prompt();
        cmds = tokenizer(input);
        parser(cmds);
        print_token_list(cmds);
        free_token_list(cmds);
        free(input);
    }
    return 0;
}
