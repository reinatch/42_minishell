/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amenses- <amenses-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/14 13:47:01 by amitcul           #+#    #+#             */
/*   Updated: 2023/06/08 18:00:17 by amenses-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "../libft/includes/libft.h"

# include <stdbool.h>
# include <limits.h>
# include <stdio.h>
# include <stddef.h>

# include <readline/readline.h>
# include <readline/history.h>

# include "lexer.h"
# include "parser.h"
# include "expander.h"
# include "utils.h"
# include "builtins.h"
# include "structs.h"
# include "error.h"
# include "executor.h"

// typedef struct s_app		t_app;
// typedef struct s_tree		t_tree;
// typedef struct s_parser		t_parser;
// typedef struct s_token		t_token;
// typedef struct s_lexer		t_lexer;
// typedef struct s_env_list	t_env_list;

// struct s_env_list
// {
// 	char				*key;
// 	char				*value;
// 	struct s_env_list	*next;
// };

// struct s_app
// {
// 	t_env_list	*env_list;
// 	char		**envp;
// };

void	fill_env_list(t_app *self, char **envp);
void	free_env_list(t_app *self);
void	remove_list_item_by_key(t_app *self, char *key);
int	env(t_app **self, char **args);

#endif
