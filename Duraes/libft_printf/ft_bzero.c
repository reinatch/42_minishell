/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_bzero.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-oli <joao-oli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 11:51:49 by joao-oli          #+#    #+#             */
/*   Updated: 2023/10/15 11:21:04 by joao-oli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include <stdio.h>
#include "libft.h"

void	ft_bzero(void *s, size_t n)
{
	char	*pointer;
	size_t	i;

	pointer = (char *)s;
	i = 0;
	while (i < n)
	{
		pointer[i] = '\0';
		i++;
	}
}

// int main()
// {
//     char s[] = "Hello";
//     size_t n = 2;
//     ft_bzero(&s[1], n);
//     printf("%s\n", s);
//     printf("%c", s[3]);
// }
