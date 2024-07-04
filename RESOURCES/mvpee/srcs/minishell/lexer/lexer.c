/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvpee <mvpee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 18:18:05 by mvpee             #+#    #+#             */
/*   Updated: 2024/03/14 22:22:53 by mvpee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/minishell.h"

static void	ft_redirection_check(char ***split_temp, t_node **node)
{
	if (*split_temp)
	{
		ft_free(1, &(*node)->name);
		(*node)->name = ft_strdup(*split_temp[0]);
		ft_free_matrix(1, split_temp);
	}
}

static void	ft_expand_redirection(t_lexer *lexer, t_env *env, t_data data)
{
	t_node	*node;
	char	**split_temp;

	node = lexer->head;
	while (node)
	{
		if (node->token == INPUT || node->token == OUTPUT
			|| node->token == APPEND)
		{
			if (!ft_strcmp(node->name, "\'\'") || !ft_strcmp(node->name, \
					"\"\""))
			{
				ft_free(1, &node->name);
				node->name = ft_strdup("");
			}
			else
			{
				split_temp = ft_expand(node->name, env, data);
				ft_redirection_check(&split_temp, &node);
			}
		}
		node = node->next;
	}
}

t_lexer	*ft_lexer(char *line, t_data *data, t_env *env)
{
	char	**split;
	t_lexer	*lexer;
	int		count;
	int		i;

	count = 0;
	lexer = NULL;
	if (!line)
		return (NULL);
	if (!ft_strcmp(line, "\0") || syntax_check(line))
		return (ft_free(1, &line), NULL);
	split = ft_splittrim(get_cmd_splitted(line, &count), " ");
	if (!split)
		return (ft_free(1, &line), NULL);
	lexer = malloc(sizeof(t_lexer) * count);
	if (!lexer)
		return (ft_free_matrix(1, &split), ft_free(1, &line), NULL);
	data->nbr_cmd = count;
	i = -1;
	while (++i < count)
	{
		lexer[i] = set_lexer(split[i]);
		ft_expand_redirection(&lexer[i], env, *data);
	}
	return (ft_free_matrix(1, &split), ft_free(1, &line), lexer);
}
