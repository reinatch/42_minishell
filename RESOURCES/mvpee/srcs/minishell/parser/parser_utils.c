/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvpee <mvpee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/08 17:26:27 by mvpee             #+#    #+#             */
/*   Updated: 2024/03/14 09:49:59 by mvpee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/minishell.h"

t_parser	init_parser(void)
{
	t_parser	parser;

	parser.cmd = NULL;
	parser.input = -1;
	parser.output = -1;
	parser.isbuiltins = false;
	parser.ft_isspecial = false;
	parser.path = NULL;
	parser.flag = false;
	return (parser);
}

void	free_parser(t_parser *parser, t_data data)
{
	int	i;

	i = -1;
	while (++i < data.nbr_cmd)
	{
		ft_free_matrix(1, &parser[i].cmd);
		ft_free(1, &parser[i].path);
		if (parser[i].input != -1)
			close(parser[i].input);
		if (parser[i].output != -1)
			close(parser[i].output);
	}
	ft_free(1, &parser);
}
