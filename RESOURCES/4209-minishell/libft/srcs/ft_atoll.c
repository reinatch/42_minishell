/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoll.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoda-s <antoda-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/05 23:53:58 by antoda-s          #+#    #+#             */
/*   Updated: 2024/03/25 17:14:09 by antoda-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

/// @see		ft_isspace, ft_issign, ft_isdigit
/// @brief 		Converts the initial portion of the string pointed to by nptr
///				to long int
/// @param nptr	The string to convert
/// @return		The converted long int
long double	ft_atoll(const char *nptr)
{
	long double	signal;
	long double	number;

	signal = 1.0;
	number = 0.0;
	while (ft_isspace(*nptr) || ft_issign(*nptr == 1))
		nptr++;
	if (ft_issign(*nptr))
		signal *= (long double)ft_issign(*nptr++);
	while (ft_isdigit(*nptr))
		number = number * 10.0 + (long double)(*nptr++ - '0');
	number *= signal;
	return (number);
}
