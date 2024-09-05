
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "../libft/includes/libft.h"
#include "../libft/includes/ft_printf.h"
#include <readline/readline.h>
#include <readline/history.h>

// Error handling constants
#define QUOTES "\"\'"
#define ERROR_TITLE "minishell"
#define ERROR_SYNTAX "syntax error"
#define YES 1
#define NO 0


typedef enum {
    TOKEN_WORD,
    TOKEN_REDIRECT_IN,
    TOKEN_REDIRECT_OUT,
    TOKEN_REDIRECT_APPEND,
    TOKEN_PIPE,
    TOKEN_EOF,
    TOKEN_HEREDOC
} TokenType;

typedef struct Token {
    TokenType type;
    char **to_execute;
	char *input_file;
	char *output_file;
	int append;
    char *heredoc_delimiter;
    char *heredoc_content;
	int pipe_to_next_token;
    struct Token *next;
} Token;






//--------------------------------------------------------------------------------------------
//parser


Token *new_token(TokenType type, char **value);
int find_next_quote(char *line, int start, int *count, char quote);
void free_token_list(Token *head);
int check_quotes(char *line);
void print_error(char *msg, char *key, int exit_code);
Token *tokenizer(char *input);
void parser(Token *head);
int is_valid_input(char *input);
int have_only_spaces(char *input);
int check_redirections(Token *head);
char *ft_prompt();
int process_input(char *input);
void sig_handler(int signum);

char *expander(char *input, char **envp);


void print_token_list(Token *head);








//--------------------------------------------------------------------------------------------
//execute

void free_list(Token *cmd);
void close_fds(int *fd);
char *find_path(char **envp);
char *ft_prompt();
int there_is_pipes(Token *cmds);
void free_a_arrays(char **str);
char *command_path(char **cmd, char **env);
void child_process(Token *cmds, char **env);
void ft_cd_builtin(Token *cmds);
void ft_pwd_builtin(Token *cmd);
int is_n_flag(char **to_execute);
void decide_in_and_out(Token *cmd, int **fds);
void ft_echo_builtin(Token *cmd);
void ft_env_builtin(Token *cmd, char **env);
void write_in_fd(char *str, int fd);
int number_of_strs(char **env);
void sort_pos_of_envp(char **envp, int **pos_sorted, int len);
void ft_env_sorted(char **envp, int fd);
void print_export_error(char *str, int fd);
int valid_export_args(char **to_execute, int fd);
int exec_has_repeated_var(char *exec, char *env_var);
char **ft_array_strdup(char **envp);
char **ft_envp_with_new_str(char **envp, int n_strs, char **to_execute);
char **ft_export_builtin(Token *cmd, char **envp);
char **ft_unset_builtin(char **to_execute, char **envp);
void no_pipes(Token *cmds, char **envp);
void with_pipes(Token *cmds, char **envp);
void after_receiving_cmds(Token *cmds, char **envp);
