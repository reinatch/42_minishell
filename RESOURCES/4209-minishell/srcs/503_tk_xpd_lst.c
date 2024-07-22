/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   503_tk_xpd_lst.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoda-s <antoda-s@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/29 19:10:37 by antoda-s          #+#    #+#             */
/*   Updated: 2024/03/25 15:02:57 by antoda-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/// @attention	>token builder< set of functions
/// @brief 		Creates a new token and adds it to the given token list
/// @param str	String to be transformed into a toke
/// @param size	Token size
/// @param type	Token type (as per enum t_token_type see tk_type_getter
/// @return		pointer to the new added token
t_token	*tk_addnew(const char *str, int size, t_token_type type)
{
	t_token	*tk;

	tk = ft_calloc(1, sizeof(t_token));
	if (!tk)
	{
		error_return("", errno, 1);
		return (NULL);
	}
	tk->content = ft_substr(str, 0, size);
	tk->size = size;
	tk->type = type;
	if (tk->content[0] == '\"' || tk->content[0] == '\''
		|| ft_strchr(tk->content, '='))
		tk->rm = 0;
	else
		tk->rm = 1;
	tk->next = NULL;
	return (tk);
}

/// @attention	>token builder< set of functions
/// @brief 			Gets pointer to last node of token list
/// @param head		Pointer to token list
/// @return			last token list node
t_token	*tk_lst_last_getter(t_token *ltk)
{
	if (!ltk)
		return (NULL);
	while (ltk)
	{
		if ((*ltk).next == NULL)
			return (ltk);
		ltk = (*ltk).next;
	}
	return (ltk);
}

/// @attention	>token builder< set of functions
/// @brief 		Adds a new token to the end of the token list
/// @param ltk	Pointer to token list
/// @param ntk	Pointer to new token
void	tk_lst_addback(t_token **ltk, t_token *ntk)
{
	t_token	*token_lst_last;

	if (*ltk)
	{
		token_lst_last = tk_lst_last_getter(*ltk);
		(*token_lst_last).next = &*ntk;
	}
	else
		*ltk = ntk;
}
