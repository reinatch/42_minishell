/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putchar_fd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-oli <joao-oli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 11:56:52 by joao-oli          #+#    #+#             */
/*   Updated: 2023/10/13 12:01:07 by joao-oli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
// #include <stdio.h>
// #include <fcntl.h>

void	ft_putchar_fd(char c, int fd)
{
	if (fd != -1)
	{
		write(fd, &c, 1);
	}
}

// int main()
// {
//     int fd = open("Texto.txt", O_CREAT | O_RDWR);
//     ft_putchar_fd('c', fd);
// }
