/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amitcul <amitcul@student.42porto.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/03 21:44:17 by amitcul           #+#    #+#             */
/*   Updated: 2023/06/05 20:53:07 by amitcul          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/expander.h"

size_t	loop_if_sign(t_app *app, char *str, char **t1, size_t start)
{
	int		i;
	int		ret;
	char	*t2;
	char	*t3;

	i = 0;
	ret = 0;
	while (app->envp[i])
	{
		if (ft_strncmp(str + i + 1, app->envp[i], equal_sign(app->envp[i]) - 1)
			&& after_dol_length(str, start) - start == equal_sign(app->envp[i]))
		{
			t2 = ft_strdup(app->envp[i] + equal_sign(app->envp[i]));
			t3 = ft_strjoin(*t1, t2);
			free(*t1);
			*t1 = t3;
			free(t2);
			ret = equal_sign(app->envp[i]);
		}
		i++;
	}
	if (ret == 0)
		ret = after_dol_length(str, start) - start;
	return (ret);
}

char	*char_to_string(char c)
{
	char	*str;

	str = ft_calloc(sizeof(char), 2);
	str[0] = c;
	return (str);
}

char	*detect_sign(t_app *app, char *str)
{
	size_t	i;
	char	*t1;
	char	*t2;
	char	*t3;

	i = 0;
	t1 = ft_strdup("\0");
	while (str[i])
	{
		i += handle_after_sign(i, str);
		if (str[i] == '$' && str[i + 1] == '?')
			i += question_mark(&t1);
		else if (str[i] == '$' && str[i + 1] != ' '
			&& (str[i + 1] != '"' || str[i + 2] != '\0') && str[i + 1] != '\0')
			i += loop_if_sign(app, str, &t1, i);
		else
		{
			t2 = char_to_string(str[i++]);
			t3 = ft_strjoin(t1, t2);
			free(t1);
			t1 = t3;
			free(t2);
		}
	}
	return (t1);
}

char	*delete_quotes(char *str, char c)
{
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	while (str[i])
	{
		if (str[i] == c)
		{
			j = 0;
			while (str[i + j] == c)
				j++;
			ft_strlcpy(&str[i], &str[i + j], ft_strlen(str) - i);
		}
		i++;
	}
	return (str);
}

char	**expander(t_app *app, char **str)
{
	int		i;
	char	*tmp;

	tmp = NULL;
	i = 0;
	while (str[i] != NULL)
	{
		if (sign(str[i]) != 0 && str[i][sign(str[i])]
			&& str[i][sign(str[i]) - 2] != '\'')
		{
			tmp = detect_sign(app, str[i]);
			free(str[i]);
			str[i] = tmp;
		}
		i++;
	}
	return (str);
}

//char	*expander_str(t_app *app, char *str)
//{
//	char	*tmp;
//
//	tmp = NULL;
//	if (str[sign(str) - 2] != '\'' && sign(str) != 0
//		&& str[sign(str)] != '\0')
//	{
//		tmp = detect_sign(app, str);
//		free(str);
//		str = tmp;
//	}
//	str = delete_quotes(str, '\"');
//	str = delete_quotes(str, '\'');
//	return (str);
//}
