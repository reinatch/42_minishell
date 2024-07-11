/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_perrors_exit.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rerodrig <rerodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/24 11:45:24 by rerodrig          #+#    #+#             */
/*   Updated: 2024/05/24 11:57:38 by rerodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/libft.h"
#include "../../includes/ft_printf.h"

void	ft_perrors_exit(char *str)
{
	ft_printf("%s\n", str);
	exit(EXIT_FAILURE);
}
