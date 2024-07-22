#ifndef EXPANDER_H
#define EXPANDER_H

# include "minishell.h"
size_t	sign(char *str);

size_t	handle_after_sign(size_t start, char *str);

size_t	question_mark(char **tmp);

size_t	after_dol_length(char *str, size_t start);

size_t	loop_if_sign(t_app *app, char *str, char **t1, size_t start);

char	*char_to_string(char c);

char	*detect_sign(t_app *app, char *str);

char	*delete_quotes(char *str, char c);

char	**expander(t_app *app, char **str);

size_t	handle_after_sign(size_t start, char *str);

size_t	question_mark(char **tmp);

size_t	equal_sign(char *str);

// size_t	after_dol_length(char *str, size_t start);



#endif
