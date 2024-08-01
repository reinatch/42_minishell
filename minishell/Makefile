# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rerodrig <rerodrig@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/04/17 12:22:55 by rerodrig          #+#    #+#              #
#    Updated: 2024/07/22 10:00:00 by rerodrig         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME        = minishell
LIBFT       = libft.a

LFT_PATH    = ./libft/
INCLUDES    = ./include
SRCDIR      = ./src/
BUILD_DIR   = ./build/
RM          = rm -rf

SRC         = $(wildcard $(SRCDIR)*.c)
OBJ         = $(addprefix $(BUILD_DIR), $(notdir $(SRC:.c=.o)))

# Compilation flags
CF          = -Wall -Wextra -Werror -g
CC          = cc
CFI         = -I$(INCLUDES)
LEAKS       = valgrind
LEAKS_FILE  = valgrind-out.txt
LF          = --leak-check=full \
               --show-leak-kinds=all \
               --track-origins=yes \
               --verbose \
               --log-file=$(LEAKS_FILE) \
               ./minishell

# Colors
GRN         = \e[38;5;118m
YLW         = \e[38;5;226m
RED         = \e[38;5;196m
BLU         = \e[38:5:31m
WTH         = \e[0m
CYN         = \33[1;36m
RESET       = \033[0m
BOLD        = \033[1m



all: $(LIBFT) $(NAME)
	# @printf "\n $(CYN)Generating $(NAME) executable...$(WTH)\n"
	@$(SHOW_PROGRESS)
	# @$(SHOW_LIBFT_PROGRESS)
	@printf "\n$(GRN)$(NAME) executable ready!$(WTH)\n\n"

$(NAME): $(OBJ)
	$(CC) $(CF) -o $(NAME) $(OBJ) -L $(LFT_PATH) -lft -lreadline
	@printf "$(GRN)*$(WHT)\n"

mkbuilddir:
	@mkdir -p $(BUILD_DIR)
	# @printf "\n$(_INFO) $(CYN)Compiling source files...$(WTH)\n"


$(BUILD_DIR)%.o: $(SRCDIR)%.c | mkbuilddir
	$(CC) $(CF) -c -I $(INCLUDES) $< -o $@
	# @$(SHOW_PROGRESS)
	@printf "$(YLW)º$(WHT)\n"

$(LIBFT):
	@printf "\n$(CYN)Generating Libft...$(WTH)\n"
	@$(SHOW_LIBFT_PROGRESS)
	@make --no-print-directory -C $(LFT_PATH)
	@printf "$(GRN)Libft created!$(WTH)\n\n"

re: fclean all

leaks:
	$(LEAKS) $(LF)
	@printf "$(GRN)Leaks log ready! Check $(LEAKS_FILE) $(WTH)\n\n"

cleanleaks:
	@$(RM) $(LEAKS_FILE)

clean:
	@printf "\n$(YLW)Cleaning all object files from project...$(WTH)\n"
	@$(RM) $(BUILD_DIR)
	@printf "$(GRN)All object files removed!$(WTH)\n\n"

fclean: clean
	@printf "\n$(YLW)Cleaning $(NAME) and additional files...$(WTH)\n"
	@$(RM) $(NAME)
	@printf "$(GRN)$(NAME) and additional files removed!$(WTH)\n\n"
	@printf "$(YLW)Cleaning Libft...$(WTH)\n"
	@make --no-print-directory fclean -C $(LFT_PATH)
	@printf "$(GRN)All libraries removed!$(WTH)\n\n"
	@printf "$(YLW)Cleaning Leaks log file.$(WTH)\n"
	@make -s cleanleaks
	@printf "$(GRN)Leaks log file deleted.$(WTH)\n\n"


# Define the progress bar function for compilation
define SHOW_PROGRESS
	@total=$$(echo $(SRC) | wc -w); \
	counter=0; \
	for file in $(SRC); do \
		counter=$$((counter + 1)); \
		percent=$$((counter * 100 / total)); \
		progress=$$(printf "%0.sº" $$(seq 1 $$((percent / 2)))); \
		printf "\r[%-50s] %d%%" "$$progress" "$$percent"; \
		sleep 0.1; \
	done; \
	printf "\n"
endef

# Define the progress bar function for libft
define SHOW_LIBFT_PROGRESS
	@total=$$(find $(LFT_PATH) -name '*.c' | wc -l); \
	counter=0; \
	while [ $$counter -lt $$total ]; do \
		printf "\r[%-50s] %d%%" $$(printf "%0.sº" $$(seq 1 $$(($$counter * 50 / $$total)))) $$(($$counter * 100 / $$total)); \
		sleep 0.1; \
		counter=$$((counter + 1)); \
	done; \
	printf "\n"
endef

.PHONY: all mkbuilddir clean fclean re leaks cleanleaks 

.SILENT:
