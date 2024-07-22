/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoda-s <antoda-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/14 00:28:11 by antoda-s          #+#    #+#             */
/*   Updated: 2024/02/25 17:12:02 by antoda-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

/// @functions	ft_strchar, ft_substr, ft_strdup, ft_strlen
/// @brief 		Allocates (with malloc(3)) and returns a copy of `s1` with the
/// 			characters specified in `set` removed from the beginning and the
/// 			end of the string.
/// @param s1	The string to be trimmed
/// @param set	The reference set of characters to trim
/// @return		The trimmed string, or NULL if the allocation fails
char	*ft_strtrim(char const *s1, char const *set)
{
	size_t	len;
	char	*p;

	if (!s1 || !set)
		return (NULL);
	while (ft_strchr(set, *s1) && *s1)
		s1++;
	if (!*s1)
		return (ft_strdup(""));
	len = ft_strlen(s1);
	while (ft_strchr(set, s1[len - 1]))
		len--;
	p = ft_substr(s1, 0, len);
	return (p);
}

/// @functions	ft_strchar, ft_substr, ft_strdup, ft_strlen
/// @brief 		Allocates (with malloc(3)) and returns a copy of `s1` with the
/// 			characters specified in `set` removed from the beginning
/// 			of the string to be trimmed.
/// @param s1	The string to be trimmed
/// @param set	The reference set of characters to trim
/// @return		The trimmed string, or NULL if the allocation fails
char	*ft_strtrim_leading(char const *s1, char const *set)
{
	size_t	len;
	char	*p;

	if (!s1 || !set)
		return (NULL);
	while (ft_strchr(set, *s1) && *s1)
		s1++;
	if (!*s1)
		return (ft_strdup(""));
	len = ft_strlen(s1);
	p = ft_substr(s1, 0, len);
	return (p);
}

/// @functions	ft_strchar, ft_substr, ft_strdup, ft_strlen
/// @brief 		Allocates (with malloc(3)) and returns a copy of `s1` with the
/// 			characters specified in `set` removed from end
/// 			of the string to be trimmed.
/// @param s1	The string to be trimmed
/// @param set	The reference set of characters to trim
/// @return		The trimmed string, or NULL if the allocation fails
char	*ft_strtrim_trailing(char const *s1, char const *set)
{
	size_t	len;
	char	*p;

	if (!s1 || !set)
		return (NULL);
	len = ft_strlen(s1);
	while (ft_strchr(set, s1[len - 1]))
		len--;
	p = ft_substr(s1, 0, len);
	return (p);
}
