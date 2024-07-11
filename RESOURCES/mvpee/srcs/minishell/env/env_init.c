/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nechaara <nechaara@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/13 13:19:21 by nechaara          #+#    #+#             */
/*   Updated: 2024/03/15 17:28:19 by nechaara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/minishell.h"

static t_env	*create_null_node(char *entry)
{
	t_env	*created_node;

	created_node = (t_env *)malloc(sizeof(t_env));
	if (!created_node)
		return (NULL);
	created_node->key = ft_strdup(entry);
	created_node->value = NULL;
	created_node->next = NULL;
	created_node->prv = NULL;
	return (created_node);
}

static t_env	*create_node(char *entry)
{
	t_env	*created_node;
	char	**splitted_arguments;

	if (!entry)
		return (NULL);
	splitted_arguments = env_split(entry);
	if (!splitted_arguments)
		return (create_null_node(entry));
	created_node = (t_env *)malloc(sizeof(t_env));
	if (!created_node)
		return (ft_free_matrix(1, &splitted_arguments), NULL);
	created_node->key = NULL;
	if (splitted_arguments[0])
		created_node->key = ft_strdup(splitted_arguments[0]);
	created_node->value = NULL;
	if (splitted_arguments[1])
		created_node->value = ft_strtrim(splitted_arguments[1], " ");
	created_node->next = NULL;
	created_node->prv = NULL;
	ft_free_matrix(1, &splitted_arguments);
	return (created_node);
}

t_env	*env_add_entry(t_env *head, char *entry)
{
	t_env	*new_element;
	t_env	*current;

	new_element = create_node(entry);
	if (!new_element)
		return (NULL);
	if (!head)
		return (new_element);
	current = head;
	while (current->next)
		current = current->next;
	current->next = new_element;
	new_element->prv = current;
	return (head);
}

t_env	*env_remove_entry(t_env **head, char *key)
{
	t_env	*previous_entry;
	t_env	*current_entry;
	t_env	*next_entry;

	current_entry = find_key(*head, key);
	if (!current_entry)
		return (*head);
	if (*head == current_entry)
	{
		*head = current_entry->next;
		current_entry->prv = NULL;
		current_entry = NULL;
	}
	else
	{
		previous_entry = current_entry->prv;
		next_entry = current_entry->next;
		if (previous_entry)
			previous_entry->next = next_entry;
		if (next_entry)
			next_entry->prv = previous_entry;
	}
	free_env_element(current_entry);
	return (*head);
}

t_env	*env_init(char **envs)
{
	t_env	*head;
	int		i;

	head = NULL;
	i = 0;
	if (*envs)
	{
		while (envs[i])
		{
			head = env_add_entry(head, envs[i]);
			i++;
		}
	}
	else
		head = minimal_env();
	shell_lvl_handler(head);
	return (head);
}
