NAME = minishell

LIBFT = srcs/libft

SRCS = 	main.c \
		srcs/minishell/env/env_init.c \
		srcs/minishell/env/env_operations.c \
		srcs/minishell/env/env_conversion.c \
		srcs/minishell/env/env_utils.c \
		srcs/minishell/builtins/builtins.c \
		srcs/minishell/builtins/echo.c \
		srcs/minishell/builtins/env.c \
		srcs/minishell/builtins/pwd.c \
		srcs/minishell/builtins/unset.c \
		srcs/minishell/builtins/export.c \
		srcs/minishell/builtins/exit.c \
		srcs/minishell/builtins/utils/export_utils.c \
		srcs/minishell/builtins/utils/export_sort.c \
		srcs/minishell/builtins/cd.c \
		srcs/minishell/executor/executor.c \
		srcs/minishell/executor/executor_utils.c \
		srcs/minishell/executor/child_executor.c \
		srcs/minishell/lexer/lexer.c \
		srcs/minishell/lexer/set_lexer.c \
		srcs/minishell/lexer/lexer_utils.c \
		srcs/minishell/lexer/syntax/syntax_check.c \
		srcs/minishell/lexer/syntax/syntax_utils.c \
		srcs/minishell/signal/init_signal.c \
		srcs/minishell/parser/parser.c \
		srcs/minishell/parser/parser_utils.c \
		srcs/minishell/parser/path_checker.c \
		srcs/minishell/parser/file_checker.c \
		srcs/minishell/parser/ft_heredoc.c \
		srcs/minishell/parser/expand/ft_expand.c \
		srcs/minishell/parser/expand/ft_expand_env.c \
		srcs/minishell/parser/expand/ft_expand_home.c \

OBJS = $(SRCS:.c=.o)

CFLAGS = -I $(LIBFT) -g -I /Users/$(shell whoami)/.brew/opt/readline/include -g  #-Wall -Werror -Wextra #-ggdb3 -fsanitize=address
LDFLAGS = -lft -lreadline -L $(LIBFT) -g -L /Users/$(shell whoami)/.brew/opt/readline/lib -g #-ggdb3 -fsanitize=address

RED=\033[0;31m
GREEN=\033[0;32m
NC=\033[0m

all: $(NAME)

$(LIBFT)/libft.a:
	$(MAKE) -C $(LIBFT) 

$(NAME): $(OBJS) $(LIBFT)/libft.a
	@gcc $(OBJS) $(LDFLAGS) -o $(NAME)
	@echo "\n${RED}███╗   ███╗██╗███╗   ██╗██╗███████╗██╗  ██╗███████╗██╗     ██╗     ${NC}"
	@echo   "${RED}████╗ ████║██║████╗  ██║██║██╔════╝██║  ██║██╔════╝██║     ██║     ${NC}"
	@echo   "${RED}██╔████╔██║██║██╔██╗ ██║██║███████╗███████║█████╗  ██║     ██║     ${NC}"
	@echo   "${RED}██║╚██╔╝██║██║██║╚██╗██║██║╚════██║██╔══██║██╔══╝  ██║     ██║     ${NC}"
	@echo   "${RED}██║ ╚═╝ ██║██║██║ ╚████║██║███████║██║  ██║███████╗███████╗███████╗${NC}"
	@echo   "${RED}╚═╝     ╚═╝╚═╝╚═╝  ╚═══╝╚═╝╚══════╝╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝${NC}"
	@echo   "${GREEN}./minishell${NC}\n"

%.o: %.c
	@gcc $(CFLAGS) -c $< -o $@

clean:
	$(MAKE) -C $(LIBFT) clean
	@rm -f $(OBJS)
	@rm -f .heredoc

fclean: clean
	$(MAKE) -C $(LIBFT) fclean
	@rm -f $(NAME)

re: fclean all

run: all
	./$(NAME)

.PHONY: all clean fclean re