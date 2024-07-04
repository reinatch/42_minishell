/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvpee <mvpee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 14:16:17 by nechaara          #+#    #+#             */
/*   Updated: 2024/03/16 15:33:13 by mvpee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/minishell.h"

char	**env_split(char *env)
{
	char	**result;
	char	*split;
	size_t	len;

	split = ft_strchr(env, '=');
	if (!split || split == env)
		return (NULL);
	len = split - env;
	result = (char **)malloc(3 * sizeof(char *));
	if (!result)
		return (NULL);
	result[0] = ft_strndup(env, len);
	if (!result[0])
		return (free(result), NULL);
	result[1] = ft_strdup(split + 1);
	result[2] = NULL;
	return (result);
}

static void	apply_shlvl_value(t_env *head, t_env *target_node)
{
	int		target_node_value;
	char	*string_entry;
	char	*entry;

	if (!target_node)
		return ;
	target_node_value = ft_atoi(target_node->value) + 1;
	env_remove_entry(&head, SHLVL_KEY);
	if (target_node_value < 0)
		env_add_entry(head, SHLVL_ZERO);
	else if (target_node_value > 999)
		env_add_entry(head, SHLVL_EMPTY);
	else
	{
		string_entry = ft_itoa(target_node_value);
		entry = ft_strjoin(SHLVL_EMPTY, string_entry);
		env_add_entry(head, entry);
		ft_free(2, &entry, &string_entry);
	}
}

void	shell_lvl_handler(t_env *head)
{
	t_env	*target_node;

	target_node = find_key(head, SHLVL_KEY);
	if (!target_node)
		head = env_add_entry(head, BASE_SHLVL);
	else
	{
		if (!ft_is_string_number(get_value(target_node)))
		{
			env_remove_entry(&head, SHLVL_KEY);
			head = env_add_entry(head, BASE_SHLVL);
		}
		else
			apply_shlvl_value(head, target_node);
	}
}

char	*no_null_join(char *s1, char *s2)
{
	int		i;
	int		j;
	char	*str;

	str = (char *)malloc(sizeof(char) * (ft_strlen(s1) + ft_strlen(s2) + 1));
	if (!str)
		return (NULL);
	i = -1;
	j = 0;
	if (s1)
	{
		while (s1[++i])
			str[j++] = s1[i];
	}
	i = -1;
	if (s2)
	{
		while (s2[++i])
			str[j++] = s2[i];
	}
	str[j] = '\0';
	return (str);
}

t_env	*minimal_env(void)
{
	t_env	*head;
	char	buffer[500];
	char	*current_working_directory;

	current_working_directory = ft_strdup(getcwd(buffer, 500));
	head = NULL;
	head = env_add_entry(head, "PWD=");
	env_add_entry(head, "OLDPWD");
	env_add_entry(head, "SHLVL=");
	write_value(&head, "PWD", current_working_directory);
	ft_free(1, &current_working_directory);
	return (head);
}
