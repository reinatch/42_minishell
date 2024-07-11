/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_operations.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvpee <mvpee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/13 14:23:01 by nechaara          #+#    #+#             */
/*   Updated: 2024/03/13 10:43:18 by mvpee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/minishell.h"

t_env	*find_key(t_env *head, char *key)
{
	if (!key)
		return (NULL);
	while (head)
	{
		if (!ft_strcmp(head->key, key))
			return (head);
		head = head->next;
	}
	return (NULL);
}

char	*get_value(t_env *target_node)
{
	if (target_node)
		return (target_node->value);
	return (NULL);
}

void	write_value(t_env **head, char *key, char *value)
{
	t_env	*target_node;
	char	*initial_string;
	char	*string_to_join;

	if (!value)
		return ;
	target_node = find_key(*head, key);
	if (!target_node)
	{
		string_to_join = reconstructed_entry(key, value);
		env_add_entry(*head, string_to_join);
		ft_free(1, &string_to_join);
		return ;
	}
	initial_string = target_node->value;
	string_to_join = no_null_join(initial_string, value);
	free(initial_string);
	target_node->value = string_to_join;
}

void	free_env_element(t_env *node)
{
	if (!node)
		return ;
	if (node->key)
		free(node->key);
	if (node->value)
		free(node->value);
	free(node);
}

void	free_env_list(t_env *head)
{
	t_env	*tmp;

	while (head)
	{
		tmp = head;
		head = head->next;
		free_env_element(tmp);
		tmp = NULL;
	}
}
