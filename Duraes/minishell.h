#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <fcntl.h>
#include "libft_printf/libft.h"
#include <errno.h>

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

