/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-oli <joao-oli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 11:57:00 by joao-oli          #+#    #+#             */
/*   Updated: 2023/11/04 16:48:51 by joao-oli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdio.h>

static int	ft_count_word(char const *s, char c)
{
	int	count;
	int	i;

	i = 0;
	count = 0;
	while (s[i])
	{
		if (s[i] != c)
		{
			count++;
			while (s[i] && s[i] != c)
				i++;
		}
		else
			i++;
	}
	return (count);
}

static int	ft_len_word(char const *s, char c, int index)
{
	int	count;
	int	i;
	int	start;
	int	tam;

	i = 0;
	count = 0;
	while (s[i])
	{
		if (s[i] != c)
		{
			count++;
			start = i;
			while (s[i] && s[i] != c)
				i++;
			tam = i - start;
			if (count == index + 1)
				return (tam);
		}
		else
			i++;
	}
	return (0);
}

char	*ft_get_word(char const *s, char c, int index, char *word)
{
	int	count;
	int	i;
	int	t;

	i = 0;
	t = 0;
	count = 0;
	while (s[i])
	{
		if (s[i] != c)
		{
			count++;
			while (s[i] && s[i] != c)
			{
				if (count == index + 1)
					word[t++] = s[i];
				i++;
			}
		}
		else
			i++;
	}
	word[t] = '\0';
	return (word);
}

void	ft_free(int i, char **words)
{
	while (i > 0)
		free(words[--i]);
	free(words);
}

char	**ft_split(char const *s, char c)
{
	char	**words;
	int		i;

	words = malloc((sizeof(char *) * (ft_count_word(s, c) + 1)));
	if (!words)
		return (NULL);
	i = 0;
	while (i < ft_count_word(s, c))
	{
		words[i] = malloc(ft_len_word(s, c, i) + 1);
		if (!words[i])
		{
			ft_free(i, words);
			return (NULL);
		}
		words[i] = ft_get_word(s, c, i, words[i]);
		i++;
	}
	words[i] = NULL;
	return (words);
}

/*int main()
{
    int i = 0;
    char const str[] = "   ";
    char **array_str = ft_split(str, ' ');

    if (array_str == NULL)
    {
        printf("Memory allocation failed.\n");
        return 1;
    }

    while (array_str[i] != NULL)
    {
        printf("%s\n", array_str[i]);
        free(array_str[i]);
        i++;
    }

    free(array_str);

    return 0;
}*/
