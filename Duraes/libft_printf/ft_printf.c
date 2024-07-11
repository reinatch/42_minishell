/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-oli <joao-oli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 10:27:09 by joao-oli          #+#    #+#             */
/*   Updated: 2023/11/12 12:41:23 by joao-oli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	print_d(int n, int count)
{
	int	digit;

	if (n == -2147483648)
		return (write(1, "-2147483648", 11));
	if (n < 0)
	{
		n = -n;
		write(1, "-", 1);
		count++;
	}
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

int	print_s(char *str)
{
	int	count;

	count = 0;
	if (str == NULL)
		return (write(1, "(null)", 6));
	while (*str != '\0')
	{
		write(1, &(*str), 1);
		count++;
		str++;
	}
	return (count);
}

int	print_c(int c)
{
	write(1, &c, 1);
	return (1);
}

int	ft_format(va_list arg, char str)
{
	int	count;

	count = 0;
	if (str == 'c' || str == '%')
		count = print_c(va_arg(arg, int));
	else if (str == 's')
		count = print_s(va_arg(arg, char *));
	else if (str == 'd' || str == 'i')
		count = print_d(va_arg(arg, int), 1);
	else if (str == 'u')
		count = print_u(va_arg(arg, unsigned int), 1);
	else if (str == 'x')
		count = print_lower_x(va_arg(arg, int), 1);
	else if (str == 'X')
		count = print_upper_x(va_arg(arg, int), 1);
	else if (str == 'p')
		count = convert_p_to_nmbr(va_arg(arg, void *));
	return (count);
}

int	ft_printf(const char *str, ...)
{
	int		count;
	int		flag;
	va_list	list;

	va_start(list, str);
	count = 0;
	while (*str != '\0')
	{
		flag = 0;
		if (*str == '%' && *(++str) != '%')
		{
			count += ft_format(list, *(str));
			flag = 1;
		}
		else
		{
			write(1, &(*str), 1);
		}
		if (flag == 0)
			count++;
		str++;
	}
	va_end(list);
	return (count);
}

// int main()
// {
//     ft_printf(" %x ", -248);
// 	printf(" %x ", (unsigned int)-248);
// 	// printf(" NULL %s NULL ", NULL);
// }
