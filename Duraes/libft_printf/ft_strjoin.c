/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-oli <joao-oli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 11:57:15 by joao-oli          #+#    #+#             */
/*   Updated: 2023/11/04 15:26:30 by joao-oli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
// #include <stdio.h>

char	*ft_strjoin(char const *s1, char const *s2)
{
	int			i;
	int			j;
	char		*newstr;

	i = 0;
	newstr = (char *)malloc(sizeof(char) * (ft_strlen(s1) + ft_strlen(s2) + 1));
	if (!newstr)
		return (NULL);
	while (s1[i] != '\0')
	{
		newstr[i] = s1[i];
		i++;
	}
	j = i;
	i = -1;
	while (s2[++i] != '\0')
	{
		newstr[j] = s2[i];
		j++;
	}
	newstr[j] = '\0';
	return (newstr);
}

// int main()
// {
//     char const *s1 = "Hello ";
//     char const *s2 = "World";
//     char *newstr = ft_strjoin(s1, s2);
//     printf("%s", newstr);
//     free(newstr);
// }
