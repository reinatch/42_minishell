/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-oli <joao-oli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 11:57:40 by joao-oli          #+#    #+#             */
/*   Updated: 2023/11/04 12:28:08 by joao-oli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <string.h>
#include "libft.h"

static int	ft_begin(char const *s1, char const *set)
{
	int	i;
	int	t;
	int	flag;

	i = -1;
	while (s1[++i])
	{
		t = -1;
		flag = 1;
		while (set[++t])
			if (s1[i] == set[t])
				flag = 0;
		if (flag)
			break ;
	}
	return (i);
}

static int	ft_end(char const *s1, char const *set)
{
	int	i;
	int	t;
	int	flag;

	i = ft_strlen(s1);
	while (i-- > 0)
	{
		t = -1;
		flag = 1;
		while (set[++t])
			if (s1[i] == set[t])
				flag = 0;
		if (flag)
			break ;
	}
	return (i + 1);
}

static void	ft_fill(char const *s1, char *str, int begin, int end)
{
	int	i;

	i = 0;
	while (begin < end)
		str[i++] = s1[begin++];
	str[i] = 0;
}

char	*ft_strtrim(char const *s1, char const *set)
{
	int		begin;
	int		end;
	char	*str;

	begin = ft_begin(s1, set);
	end = ft_end(s1, set);
	if (begin > end)
		begin = end;
	str = (char *)malloc(sizeof(char) * (end - begin + 1));
	if (!str)
		return (NULL);
	ft_fill(s1, str, begin, end);
	return (str);
}

// int main()
// {
//     char s1[] = " lorem ipsum dolor sit amet";
//     char *pointer = &s1[0];
//     char set[] = "l ";
//     char *pset = &set[0];
//     char *newstr = ft_strtrim(pointer, pset);
//     printf("New string:%s", newstr);
//     free(newstr);
// }
