gcc ./src/minishell.c -o myshell -lreadline -Wall -Wextra -Werror -g

cc -Wall -Wextra -Werror -g -I./includes parsing.c ./libft/libft.a -lreadline -o parsing
