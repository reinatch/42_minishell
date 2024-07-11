/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-oli <joao-oli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 13:36:45 by joao-oli          #+#    #+#             */
/*   Updated: 2023/10/15 17:35:53 by joao-oli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_negativo_ou_positivo(int *n)
{
	if (*n < 0)
	{
		*n = -(*n);
		return (-1);
	}
	return (1);
}

void	ft_put_number_in_file(char *number_in_string, int i, int fd)
{
	while (i <= 9)
	{
		write(fd, &number_in_string[i], 1);
		i++;
	}
}

void	ft_putnbr_fd(int n, int fd)
{
	int		posi_nega;
	int		i;
	char	number_in_string[10];

	if (n == -2147483648)
		write(fd, "-2147483648", 11);
	else
	{
		i = 9;
		posi_nega = ft_negativo_ou_positivo(&n);
		if (posi_nega == -1)
			write(fd, "-", 1);
		while (n >= 10)
		{
			number_in_string[i] = (n % 10) + '0';
			n /= 10;
			i--;
		}
		number_in_string[i] = n + '0';
		ft_put_number_in_file(number_in_string, i, fd);
	}
}

// int main()
// {
//     ft_putnbr_fd(-2147483647, 1);
// }
