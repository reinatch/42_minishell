/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-oli <joao-oli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 11:57:30 by joao-oli          #+#    #+#             */
/*   Updated: 2023/10/15 12:13:02 by joao-oli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdio.h>

int	ft_strncmp(char *s1, char *s2, size_t n)
{
	size_t	i;
	int		comp;

	comp = 0;
	i = 0;
	while ((s1[i] != '\0' || s2[i] != '\0') && i < n)
	{
		comp = (unsigned char)s1[i] - (unsigned char)s2[i];
		if (comp != 0)
		{
			return (comp);
		}
		i++;
	}
	return (comp);
}

/*int main()
{
	printf("%d", ft_strncmp("test", "testss", 7));
}*/
