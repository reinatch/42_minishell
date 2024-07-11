/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nechaara <nechaara@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/11 21:18:31 by mvpee             #+#    #+#             */
/*   Updated: 2024/04/13 01:20:03 by nechaara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXECUTOR_H
# define EXECUTOR_H

void	executor(t_env **head, t_data *data, t_parser *parser);
void	child_executor(t_env **head, t_data *data, t_parser *parser);
void	clean_fds(t_data *data);

#endif