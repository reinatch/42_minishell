/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-oli <joao-oli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 11:57:33 by joao-oli          #+#    #+#             */
/*   Updated: 2023/11/01 15:55:54 by joao-oli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <string.h>
#include "libft.h"

char	*ft_strnstr(const char *big, const char *little, size_t len)
{
	size_t	i;

	i = 0;
	if (!little[0])
		return ((char *)big);
	while (i <= (len - ft_strlen(little)) && big[i])
	{
		if (!ft_strncmp(&((char *)big)[i], ((char *)little), ft_strlen(little)))
		{
			if (len >= ft_strlen(little))
				return (&((char *)big)[i]);
		}
		i++;
	}
	return (0);
}

/*int main()
{
    printf("%s", ft_strnstr("lorem ipsum dolor sit amet", "ipsumm", 30));
}*/
