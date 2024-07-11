/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvan-pee <mvan-pee@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/15 12:39:03 by nechaara          #+#    #+#             */
/*   Updated: 2024/03/18 12:02:06 by mvan-pee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_H
# define ERROR_H

// GLOBAL CODE
# define SUCCESSFUL_COMMAND 0
# define UNSUCCESSFUL_COMMAND 1
// ENV VARIABLE CODES
# define MISSING_RIGHTS 126
# define COMMAND_NOT_FOUND 127
# define COMMAND_INTERRUPTED 130
# define COMMAND_INTERRUPTED_QUIT 131
// EXIT RELATED ERROR CODES
# define NUMERICAL_ARGUMENT_NESCESSARY 255
# define TOO_MANY_ARGUMENTS 1

#endif