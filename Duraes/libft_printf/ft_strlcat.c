/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-oli <joao-oli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 11:57:18 by joao-oli          #+#    #+#             */
/*   Updated: 2023/10/15 18:18:59 by joao-oli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t size)
{
	size_t	i;
	size_t	sizedst;
	size_t	sizesrc;

	i = 0;
	sizedst = ft_strlen(dst);
	sizesrc = ft_strlen(src);
	if (sizedst >= size)
		return (size + ft_strlen(src));
	while (src[i] != '\0' && i < size - sizedst - 1)
	{
		dst[sizedst + i] = src[i];
		i++;
	}
	dst[sizedst + i] = '\0';
	return (sizedst + sizesrc);
}
