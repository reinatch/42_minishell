#include "../../includes/expander.h"

size_t	sign(char *str)
{
	size_t	i;

	i = 0;
	while (str[i])
	{
		if (str[i] == '$')
			return (i + 1);
		i++;
	}
	return (0);
}

size_t	handle_after_sign(size_t start, char *str)
{
	size_t	i;

	i = start;
	if (str[start] == '$')
	{
		if (ft_isdigit(str[start + 1]))
			start += 2;
	}
	return (start - i);
}

size_t	question_mark(char **tmp)
{
	free(*tmp);
	// *tmp = ft_itoa(global_error_code);
	return (ft_strlen(*tmp) + 1);
}

size_t	equal_sign(char *str)
{
	size_t	i;

	i = 0;
	while (str[i])
	{
		if (str[i] == '=')
			return (i + 1);
		i++;
	}
	return (0);
}

size_t	after_dol_length(char *str, size_t start)
{
	size_t	i;

	i = start + 1;
	while (str[i] != '\0' && str[i] != '$' && str[i] != ' '
		   && str[i] != '\"' && str[i] != '\'' && str[i] != '=' && str[i] != '-'
		   && str[i] != ':')
		i++;
	return (i);
}