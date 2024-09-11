#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>

typedef enum {
    TOKEN_WORD,
    TOKEN_REDIRECT_IN,
    TOKEN_REDIRECT_OUT,
    TOKEN_REDIRECT_APPEND,
    TOKEN_PIPE,
    TOKEN_HEREDOC,
    TOKEN_EOF
} t_tokenType;

typedef struct s_lexer
{
    char            *content;
    int             size;
    t_tokenType     type;
    struct s_lexer  *next;
}               t_lexer;

typedef struct s_command
{
    t_tokenType     type;
    char **to_execute;
    char *input_file;
    char *output_file;
    int append;
    char *heredoc_delimiter;
    char *heredoc_content;
    int pipe_to_next_token;
    struct s_command  *next;
}               t_command;

// Helper functions
char *ft_strdup(const char *s1) {
    char *copy = malloc(strlen(s1) + 1);
    if (copy)
        strcpy(copy, s1);
    return copy;
}

t_lexer *new_token(char *content, int size, t_tokenType type) {
    t_lexer *new = malloc(sizeof(t_lexer));
    new->content = strndup(content, size);
    new->size = size;
    new->type = type;
    new->next = NULL;
    return new;
}

void add_token(t_lexer **lexer, t_lexer *new) {
    if (*lexer == NULL) {
        *lexer = new;
    } else {
        t_lexer *temp = *lexer;
        while (temp->next)
            temp = temp->next;
        temp->next = new;
    }
}

void free_lexer(t_lexer *lexer) {
    t_lexer *temp;
    while (lexer) {
        temp = lexer;
        lexer = lexer->next;
        free(temp->content);
        free(temp);
    }
}

void free_command(t_command *cmd) {
    if (cmd->to_execute) {
        for (int i = 0; cmd->to_execute[i]; i++)
            free(cmd->to_execute[i]);
        free(cmd->to_execute);
    }
    free(cmd->input_file);
    free(cmd->output_file);
    free(cmd->heredoc_delimiter);
    free(cmd->heredoc_content);
    free(cmd);
}

void print_command(t_command *current) {
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
void tokenize(char *input, t_lexer **lexer) {
    while (*input) {
        if (isspace(*input)) {
            input++;
            continue;
        }
        if (strncmp(input, "<<", 2) == 0) {
            add_token(lexer, new_token(input, 2, TOKEN_HEREDOC));
            input += 2;
        } else if (strncmp(input, ">>", 2) == 0) {
            add_token(lexer, new_token(input, 2, TOKEN_REDIRECT_APPEND));
            input += 2;
        } else if (*input == '<') {
            add_token(lexer, new_token(input, 1, TOKEN_REDIRECT_IN));
            input++;
        } else if (*input == '>') {
            add_token(lexer, new_token(input, 1, TOKEN_REDIRECT_OUT));
            input++;
        } else if (*input == '|') {
            add_token(lexer, new_token(input, 1, TOKEN_PIPE));
            input++;
        } else {
            char *start = input;
            while (*input && !isspace(*input) && *input != '<' && *input != '>' && *input != '|')
                input++;
            add_token(lexer, new_token(start, input - start, TOKEN_WORD));
        }
    }
}
void add_command(t_command **commands, t_command *new_command) {
    if (*commands == NULL) {
        *commands = new_command;
    } else {
        t_command *temp = *commands;
        while (temp->next)
            temp = temp->next;
        temp->next = new_command;
    }
}

t_command *new_command_node() {
    t_command *cmd = malloc(sizeof(t_command));
    if (cmd) {
        cmd->type = TOKEN_WORD;
        cmd->to_execute = NULL;
        cmd->input_file = NULL;
        cmd->output_file = NULL;
        cmd->append = 0;
        cmd->heredoc_delimiter = NULL;
        cmd->heredoc_content = NULL;
        cmd->pipe_to_next_token = 0;
        cmd->next = NULL;
    }
    return cmd;
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

void parse(t_lexer *lexer, t_command **commands) {
    t_command *current_cmd = NULL;

    while (lexer) {
        if (lexer->type == TOKEN_WORD) {
            if (!current_cmd) {
                current_cmd = new_command_node();
                add_command(commands, current_cmd);
            }
            int argc = count_args(lexer);
            current_cmd->to_execute = calloc(argc + 1, sizeof(char *));
            int arg_index = 0;
            while (lexer && lexer->type != TOKEN_PIPE) {
                if (lexer->type == TOKEN_WORD) {
                    current_cmd->to_execute[arg_index++] = ft_strdup(lexer->content);
                } else if (lexer->type == TOKEN_REDIRECT_IN) {
                    lexer = lexer->next;
                    if (lexer && lexer->type == TOKEN_WORD)
                        current_cmd->input_file = ft_strdup(lexer->content);
                } else if (lexer->type == TOKEN_REDIRECT_OUT) {
                    lexer = lexer->next;
                    if (lexer && lexer->type == TOKEN_WORD)
                        current_cmd->output_file = ft_strdup(lexer->content);
                } else if (lexer->type == TOKEN_REDIRECT_APPEND) {
                    lexer = lexer->next;
                    if (lexer && lexer->type == TOKEN_WORD) {
                        current_cmd->output_file = ft_strdup(lexer->content);
                        current_cmd->append = 1;
                    }
                } else if (lexer->type == TOKEN_HEREDOC) {
                    lexer = lexer->next;
                    if (lexer && lexer->type == TOKEN_WORD) {
                        current_cmd->heredoc_delimiter = ft_strdup(lexer->content);
                        current_cmd->heredoc_content = ft_strdup("heredoc content..."); // Placeholder
                    }
                }
                lexer = lexer->next;
            }
            if (lexer && lexer->type == TOKEN_PIPE) {
                current_cmd->pipe_to_next_token = 1;
                lexer = lexer->next;
                current_cmd = NULL;  // Prepare for the next command
            }
        } else {
            lexer = lexer->next;
        }
    }
}

int main() {
    char input[] = "cat file.txt | grep 'search' > output.txt << EOF";
    t_lexer *lexer = NULL;
    t_command *commands = NULL;

    tokenize(input, &lexer);
    parse(lexer, &commands);

    print_command(commands);

    // Free memory
    t_command *current = commands;
    while (current) {
        t_command *next = current->next;
        free_command(current);
        current = next;
    }

    free_lexer(lexer);
    return 0;
}