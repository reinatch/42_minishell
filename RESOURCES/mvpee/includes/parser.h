/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvpee <mvpee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 14:01:20 by mvpee             #+#    #+#             */
/*   Updated: 2024/03/14 09:50:36 by mvpee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

# include "../srcs/libft/includes/libft.h"
# include "minishell.h"
# include <stdbool.h>

typedef struct s_parser
{
	char	**cmd;
	int		input;
	int		output;
	bool	isbuiltins;
	bool	ft_isspecial;
	bool	flag;
	char	*path;
}			t_parser;

t_parser	*ft_parser(t_lexer *lexer, t_data *data, t_env *head);
char		*path_checker(char **paths, t_parser parser);
bool		file_checker(t_parser *parser, t_lexer lexer, t_env *env,
				t_data *data);

void		ft_heredoc(int fd, char **stop, t_env *env, t_data *data);

t_parser	init_parser(void);
void		free_parser(t_parser *parser, t_data data);

char		**ft_expand(char *line, t_env *head, t_data data);
char		*exp_env(char *line, t_env *head, t_data data, int *i);
void		ft_expand_home(char *line, int *i, char **buffer);

bool		only_space(char *line);

#endif