
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
CF 			= -Wall -Wextra -Werror -g -lreadline
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
	$(CC) $(CF) -o $(NAME) $(OBJ) -L $(LFT_PATH) -lft
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

##@ Norm Rules

norm: $(TEMP_PATH)		## Run norminette test on source files
	@echo "$(CYA)$(_SEP)$(D)"
	@printf "${_NORM}: $(YEL)$(SRC_PATH)$(D)\n"
	@ls $(SRC_PATH) | wc -l > $(TEMP_PATH)/norm_ls.txt
	@printf "$(_NORM_INFO) $$(cat $(TEMP_PATH)/norm_ls.txt)\n"
	@printf "$(_NORM_SUCCESS) "
	@norminette $(SRC_PATH) | grep -wc "OK" > $(TEMP_PATH)/norm.txt; \
	if [ $$? -eq 1 ]; then \
		echo "0" > $(TEMP_PATH)/norm.txt; \
	fi
	@printf "$$(cat $(TEMP_PATH)/norm.txt)\n"
	@if ! diff -q $(TEMP_PATH)/norm_ls.txt $(TEMP_PATH)/norm.txt > /dev/null; then \
		printf "$(_NORM_ERR) "; \
		norminette $(SRC_PATH) | grep -v "OK"> $(TEMP_PATH)/norm_err.txt; \
		cat $(TEMP_PATH)/norm_err.txt | grep -wc "Error:" > $(TEMP_PATH)/norm_errn.txt; \
		printf "$$(cat $(TEMP_PATH)/norm_errn.txt)\n"; \
		printf "$$(cat $(TEMP_PATH)/norm_err.txt)\n"; \
	else \
		printf "[$(YEL)Everything is OK$(D)]\n"; \
	fi
	@echo "$(CYA)$(_SEP)$(D)"

check_ext_func: all		## Check for external functions
	@echo "[$(YEL)Checking for external functions$(D)]"
	@echo "$(YEL)$(_SEP)$(D)"
	@echo "$(CYA)Reading binary$(D): $(MAG)$(NAME)$(D)"
	nm ./$(NAME) | grep "U" | grep -v "__" | tee $(TEMP_PATH)/ext_func.txt
	@echo "$(YEL)$(_SEP)$(D)"

.PHONY: clean fclean re rebonus all bonus
