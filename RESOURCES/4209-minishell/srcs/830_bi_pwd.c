/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   830_bi_pwd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoda-s <antoda-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 23:46:04 by antoda-s          #+#    #+#             */
/*   Updated: 2024/03/25 15:02:42 by antoda-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/// @brief 			Builtin pwd command
/// @param void		Builtin command arguments not required
/// @return			SUCCESS or ERROR
int	bi_pwd(t_script *s, int n)
{
	int		i;
	char	*buf;

	(void) n;
	i = 0;
	while (s->cmds[n].argv[++i])
		if (s->cmds[n].argv[i][0] == '-')
			return (error_bi_flags(s->cmds[n].argv[0], s->cmds[n].argv[i], 0));
	buf = NULL;
	buf = getcwd(buf, MAX_PATH_LEN);
	if (!buf)
	{
		free(buf);
		buf = env_var_getter("PWD", s->envp, NULL);
		ft_putendl_fd(buf, STDOUT_FILENO);
	}
	else
		ft_putendl_fd(buf, STDOUT_FILENO);
	free(buf);
	return (SUCCESS);
}
