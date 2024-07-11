/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_heredoc.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvpee <mvpee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 14:38:04 by mvpee             #+#    #+#             */
/*   Updated: 2024/03/17 20:15:18 by mvpee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/minishell.h"

static char	*check_heredoc_stop(char **stop, bool *flag)
{
	char	*new_stop;
	int		i;

	if (!ft_strcmp(*stop, "\'\'") || !ft_strcmp(*stop, "\"\""))
	{
		ft_free(1, stop);
		return (ft_strdup(""));
	}
	new_stop = NULL;
	*flag = false;
	if (ft_ischarin('\'', *stop) || ft_ischarin('\"', *stop))
		*flag = true;
	i = -1;
	while ((*stop)[++i])
		if ((*stop)[i] != '\'' && (*stop)[i] != '\"')
			new_stop = ft_strjoinchar_free(&new_stop, (*stop)[i]);
	ft_free(1, stop);
	return (new_stop);
}

static char	*heredoc_expand(char *line, t_env *env, t_data data, int *i)
{
	char	*str;
	char	*buffer;

	str = NULL;
	if (line[*i + 1] == '?')
	{
		str = ft_strjoin_ff(&str, ft_itoa(data.env_var));
		(*i) += 2;
	}
	else if (!ft_isalnum(line[*i + 1]))
		str = ft_strjoinchar_free(&str, line[*i]);
	else
	{
		buffer = ft_strdup("");
		while (ft_isalpha(line[++(*i)]) || line[*i] == '_' || line[*i] == '?')
			buffer = ft_strjoinchar_free(&buffer, line[*i]);
		if (find_key(env, buffer))
			str = ft_strjoin_ff(&str, ft_strdup(find_key(env, \
						buffer)->value));
		ft_free(1, &buffer);
	}
	return (str);
}

static char	*heredoc_parser(char *line, t_env *env, t_data data)
{
	char	*str;
	int		i;

	if (!line || !env)
		return (NULL);
	i = -1;
	str = NULL;
	while (line[++i] && line[i])
	{
		if (line[i] == '$')
			str = ft_strjoin_ff(&str, heredoc_expand(line, env, data, \
					&i));
		if (line[i])
			str = ft_strjoinchar_free(&str, line[i]);
		if (!line[i])
			break ;
	}
	return (ft_free(1, &line), str);
}

void	heredoc(int fd, char **stop, t_env *env, t_data data)
{
	char	*line;
	bool	flag;

	signal(SIGINT, signal_heredoc);
	*stop = check_heredoc_stop(stop, &flag);
	while (1)
	{
		line = readline("> ");
		if (!line || !ft_strcmp(line, *stop))
		{
			free(line);
			break ;
		}
		if (!flag)
			line = heredoc_parser(line, env, data);
		if (line)
		{
			write(fd, line, ft_strlen(line));
			write(fd, "\n", 1);
			ft_free(1, &line);
		}
	}
	exit(0);
}

void	ft_heredoc(int fd, char **stop, t_env *env, t_data *data)
{
	int		status;
	pid_t	pid;

	if (data->flag)
		return ;
	signal(SIGINT, SIG_IGN);
	pid = fork();
	if (pid == 0)
		heredoc(fd, stop, env, *data);
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		data->env_var = WEXITSTATUS(status);
	if (data->env_var == UNSUCCESSFUL_COMMAND)
		data->flag = true;
}
