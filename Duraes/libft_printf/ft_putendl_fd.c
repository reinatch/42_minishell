/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putendl_fd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-oli <joao-oli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 12:10:53 by joao-oli          #+#    #+#             */
/*   Updated: 2023/10/13 13:27:08 by joao-oli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
// #include <fcntl.h>

void	ft_putendl_fd(char *s, int fd)
{
	int		i;
	char	newl;

	i = 0;
	newl = '\n';
	if (fd != -1 || s[i] != '\0')
	{
		while (s[i] != '\0')
		{
			write(fd, &s[i], 1);
			i++;
		}
		write(fd, &newl, 1);
	}
}

// int main(int argc, char **argv)
// {
//     (void)argc;
//     ft_putendl_fd(argv[1], 1);
// }
//int fd = open("Texto", O_CREAT | O_RDWR);
//ft_putendl_fd("Hello World", fd);
