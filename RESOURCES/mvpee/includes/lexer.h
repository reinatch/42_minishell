/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvpee <mvpee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/07 10:49:12 by mvpee             #+#    #+#             */
/*   Updated: 2024/03/13 15:37:31 by mvpee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_H
# define LEXER_H

# include "../srcs/libft/includes/libft.h"
# include "minishell.h"
# include <stdbool.h>

typedef enum e_token
{
	INPUT = 1,
	HEREDOC = 2,
	OUTPUT = 3,
	APPEND = 4
}					t_token;

typedef struct s_node
{
	char			*name;
	enum e_token	token;
	struct s_node	*next;
}					t_node;

typedef struct s_lexer
{
	char			*cmd;
	struct s_node	*head;
}					t_lexer;

t_lexer				*ft_lexer(char *line, t_data *data, t_env *head);

t_lexer				set_lexer(char *str);
void				free_lexer(t_lexer *lexer, int count);
bool				syntax_check(char *line);
char				**get_cmd_splitted(char *line, int *count);
int					number_of_cmd(char *line);
void				append_node(t_node **head, char **name, t_token token);
t_node				*new_node(char *name, t_token token);

bool				check_before(char *line, int i);
bool				check_after(char *line, int i);

#endif