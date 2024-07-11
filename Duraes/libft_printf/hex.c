/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hex.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-oli <joao-oli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/12 10:36:25 by joao-oli          #+#    #+#             */
/*   Updated: 2023/11/12 10:36:57 by joao-oli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	print_p(unsigned long long int n, int count)
{
	int		digit;
	char	*hexa;

	hexa = "0123456789abcdef";
	if (n >= 16)
	{
		count = print_p(n / 16, ++count);
		digit = hexa[n % 16];
		write(1, &digit, 1);
	}
	else
	{
		digit = hexa[n];
		write(1, &digit, 1);
	}
	return (count);
}

int	convert_p_to_nmbr(void *pointer)
{
	unsigned long long int	ptr;

	if (!pointer)
	{
		write(1, "(nil)", 5);
		return (5);
	}
	write(1, "0x", 2);
	ptr = (unsigned long long int)pointer;
	return (print_p(ptr, 1) + 2);
}
// +2 do 0x

int	print_upper_x(unsigned int n, int count)
{
	int		digit;
	char	*hexa;

	hexa = "0123456789ABCDEF";
	if (n >= 16)
	{
		count = print_upper_x(n / 16, ++count);
		digit = hexa[n % 16];
		write(1, &digit, 1);
	}
	else
	{
		digit = hexa[n];
		write(1, &digit, 1);
	}
	return (count);
}

int	print_lower_x(unsigned int n, int count)
{
	int		digit;
	char	*hexa;

	hexa = "0123456789abcdef";
	if (n >= 16)
	{
		count = print_lower_x(n / 16, ++count);
		digit = hexa[n % 16];
		write(1, &digit, 1);
	}
	else
	{
		digit = hexa[n];
		write(1, &digit, 1);
	}
	return (count);
}

int	print_u(unsigned int n, int count)
{
	int	digit;

	if (n >= 10)
	{
		count = print_d(n / 10, ++count);
		digit = (n % 10) + '0';
		write(1, &digit, 1);
	}
	else
	{
		digit = n + '0';
		write(1, &digit, 1);
	}
	return (count);
}
