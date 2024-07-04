/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvan-pee <mvan-pee@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/15 16:14:21 by nechaara          #+#    #+#             */
/*   Updated: 2024/03/15 18:26:48 by mvan-pee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ENV_H
# define ENV_H

# define SHLVL_EMPTY "SHLVL="
# define BASE_SHLVL "SHLVL=1"
# define SHLVL_KEY "SHLVL"
# define SHLVL_ZERO "SHLVL=0"

// Env Init
t_env	*env_init(char **envs);

// Env Operations
char	**env_to_tab(t_env *head);
char	**env_split(char *env);
t_env	*tab_to_env(char **envp);
t_env	*env_add_entry(t_env *head, char *entry);
t_env	*env_remove_entry(t_env **head, char *key);
t_env	*find_key(t_env *head, char *key);
char	*get_value(t_env *target_node);
t_env	*remove_top_node(t_env **head);
t_env	*get_last_entry(t_env **head);
void	write_value(t_env **head, char *key, char *value);
void	free_env_list(t_env *head);
void	shell_lvl_handler(t_env *head);
void	update_content_of_node(t_env **created_node, char **splitted_arguments);
char	*no_null_join(char *s1, char *s2);
t_env	*minimal_env(void);
void	free_env_element(t_env *node);

#endif