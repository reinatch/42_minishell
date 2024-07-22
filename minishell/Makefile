
# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rerodrig <rerodrig@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/04/17 12:22:55 by rerodrig          #+#    #+#              #
#    Updated: 2024/04/18 10:20:34 by rerodrig         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


NAME 		= minishell
LIBFT 		= libft.a

LFT_PATH 	= ./libft/
INCLUDES 	= ./include
SRCDIR 		= ./src/
BUILD_DIR   = ./build/
RM 			= rm -rf

SRC 		= $(SRCDIR)minishell.c \


OBJ			=$(notdir $(SRC:.c=.o))

#compilation
CF 			= -Wall -Wextra -Werror -g 
CC 			= cc
CFI 		= -I$(INCLUDES)
LEAKS 		= valgrind
LEAKS_FILE	= valgrind-out.txt
LF 			= --leak-check=full \
        		--show-leak-kinds=all \
        		--track-origins=yes \
        		--verbose \
        		--log-file=$(LEAKS_FILE) \
        		./minishell

all: $(NAME)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(NAME): $(BUILD_DIR) $(OBJ) $(LIBFT)
	$(CC) $(CF) -o $(NAME) $(OBJ) -L $(LFT_PATH) -lft -lreadline
	mv $(OBJ) build/

$(OBJ): $(SRC)
	$(CC) $(CF) -g -c -I ./includes/ $(SRC)

$(BUILD_DIR)%.o: $(SRCDIR)%.c | $(BUILD_DIR)
	$(CC) $(FLAGS) -c $< -o $@ -I $(INCLUDES)

$(LIBFT):
	@make -C $(LFT_PATH)

bonus: all

re: fclean all

rebonus: fclean bonus

leaks: 
	$(LEAKS) $(LF)

cleanleaks: 
	$(RM) $(LEAKS_FILE)

clean:
	@make clean -C $(LFT_PATH)
	$(RM) $(BUILD_DIR)

fclean: clean
	$(RM) -rf $(NAME)
	@make fclean -C $(LFT_PATH)
	make cleanleaks

.PHONY: clean fclean re rebonus all bonus
