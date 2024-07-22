

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
    TOKEN_EOF
} TokenType;

typedef struct Token {
    TokenType type;
    char *value;
	char *input_file;
	char *output_file;
	int append;
	int pipe_to_next_token;
    struct Token *next;
} Token;



