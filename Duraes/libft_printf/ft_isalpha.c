/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isalpha.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-oli <joao-oli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 11:51:59 by joao-oli          #+#    #+#             */
/*   Updated: 2023/10/15 11:29:09 by joao-oli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include <stdio.h>
// #include <ctype.h>
#include "libft.h"

int	ft_isalpha(int c)
{
	if ((c <= 90 && c >= 65) || (c >= 97 && c <= 122))
	{
		return (c);
	}
	else
	{
		return (0);
	}
}

// int main()
// {
//     char c = 48;
//     printf("Mine: %d\n", ft_isalpha(c));
//     printf("Original: %d", isalpha(c));
// }
