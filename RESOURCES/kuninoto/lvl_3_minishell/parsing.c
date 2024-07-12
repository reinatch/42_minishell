
#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"

# include <readline/readline.h>
# include <readline/history.h>

# include <sys/types.h>
# include <dirent.h>
# include <signal.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <errno.h>

/* CONSTANTS */

# define SUCCESS 0
# define FAILURE -1
# define OPERATORS "|<>"
# define QUOTES "\"\'"
# define DELIMS "\"\' "
# define REDIRECTS "><"

/* ERROR MESSAGES */

// MINISHELL

# define CL_ARGUMENTS_ERR "minishell: no support for command-line arguments"
# define NO_PIPE_PROMPT "minishell: no support for pipe prompt"
# define NO_SYTX_PROMPT "minishell: no support for syntax prompt"
# define PIPE_ERR "minishell: pipe() failed"
# define FORK_ERR "minishell: fork() failed"

// SYNTAX

# define UNCLOSED_QUOTES "minishell: unclosed quotes"
# define UNEXPECTED_TOKEN "minishell: syntax error near unexpected token `"
# define SYTX_ERR_RDR "minishell: syntax error near unexpected token `newline'"

// EXIT CMD

# define EXIT_TOO_MANY_ARGS "minishell: exit: too many arguments"
# define EXIT_NON_NUMERIC_ARG "minishell: exit: numeric argument required" 

// CD CMD

# define PATH_MAX	4096
# define CD_TOO_MANY_ARGS "minishell: cd: too many arguments"
# define OLDPWD_NOT_SET "minishell: cd: OLDPWD not set"

typedef enum e_operator {
	NONE,
	RDR_OUT_REPLACE,
	RDR_OUT_APPEND,
	RDR_INPUT,
	RDR_INPUT_UNTIL,
	PIPE,
}				t_operator;

/* All functions regarding t_statemnent list are
prefixed with p_ referring to parser */
typedef struct s_statement {
	int					argc;
	char				**argv;
	t_operator			operator;
	struct s_statement	*next;
}				t_statement;

/* All functions regarding t_vars 
are prefixed with v_ referring to variables */
typedef struct s_vlst {
	char			*var_name;
	char			*var_value;
	bool			is_exported;
	struct s_vlst	*next;
}				t_vlst;

/* data keeps a pointer to the head node in
 case of a need to call panic() (fork or pipe error) */

typedef struct s_data {
	char		**envp;
	t_vlst		*envp_lst;
	t_statement	*head;
}				t_data;

void				child_signals(int signum);
void				dismiss_signal(int signum);
/* Setups Minishell. 
Casts argc and argv to void
Saves envp
Initializes lsts
Configs Signals
*/
void				setup_shell(char **envp, t_data *data,
						t_statement **statement_list);

/* COMMANDS */

/* Returns true if it has executed a builtin command */
bool				builtin(t_statement *statement, t_data *data);

/* Returns true if it has sucessfully 
executed a binary from PATH */
void				cmd_binaries(t_statement *statement, t_data *data);
// Wannabe echo
int					cmd_echo(t_statement *statement, bool has_n);
// Wannabe pwd
int					cmd_pwd(void);
// Wannabe cd
// int					cmd_cd(char *path);
int					cmd_cd(char *path, t_data *data);

static inline int	cd_too_many_args(void)
{
	ft_putendl_fd(CD_TOO_MANY_ARGS, STDERR_FILENO);
	return (EXIT_FAILURE);
}
// Wannabe env
int					cmd_env(t_data *data);
// Wannabe export
int					cmd_export(t_statement *statement, t_data *data);
// Wannabe exit
// void				cmd_exit(int exit_status, t_data *data);
void				cmd_exit(t_statement *s, t_data *data);
/* Encapsulate if's. Norm Purposes */
void				call_cmd_exit(t_statement *statement, t_data *data);
// Wannabe unset
// int				cmd_unset(char *var_name, t_vlst **head);
int					unset_var(char *var_name, t_vlst **head);
int					cmd_unset(t_statement *s, t_vlst **head);

static inline void	cmd_not_found(char *cmd_name)
{
	ft_putstr_fd(cmd_name, STDERR_FILENO);
	ft_putendl_fd(": command not found", STDERR_FILENO);
}

// Utils
static inline bool	is_absolute_path(t_statement *statement)
{
	if (is_onstr(statement->argv[0], '/'))
		return (true);
	return (false);
}

bool				is_valid_id(char *str);

int					unclosed_quotes(char *str);
bool				invalid_syntax(char *input);
bool				invalid_syntax2(char *input);
bool				invalid_syntax_on_operator(char *input);
bool				valid_input(char *input, t_data *data);

bool				is_all_digits_or_signals(char *str);

void				destroy(t_data *data);

void				config_signals(void);

void				exec_cmd(t_statement *current_node, t_data *data);
void				exec_type(t_statement *statement_list, t_data *data);
void				exec_executables(t_statement *node, t_data *data);
void				exec_pipe(t_statement *node, t_data *data);
void				exec_redirects(t_statement *node, t_data *data);

// LINKED LISTS ---------------------------------

t_statement			*p_new_node(int argc);
/* Returns the size of the linked list 
which head is passed as a parameter */
size_t				p_lstsize(t_statement *head);
/* Frees the linked list which head is passed as parameter */
void				p_lstclear(t_statement **head);

t_vlst				*v_new_node(char *var_name, char *var_value,
						bool is_exported);
void				v_lstadd_back(t_vlst **head, t_vlst *new);
t_vlst				*v_lstlast(t_vlst *node);
size_t				v_lstsize_exported(t_vlst **head);
void				v_lstclear(t_vlst **head);

bool				get_exported_state(char *var_name, t_vlst **head);
char				**get_envp_arr(t_vlst **envp_lst);
char				*get_fromvlst(char *var_name, t_vlst **head);

/* same as get_fromvlst except that it
encapsulates the free procedure for <var_name> */
char				*get_varvalue_fromvlst(char *var_name, t_data *data);

char				**split_envp(char *env);
int					save_user_vars(char *statement, t_vlst **head,
						bool to_export);

t_vlst				*init_envp_lst(char **envp);

size_t				get_nr_statements(char *input);

char				*remove_quotes(char	*parsed);

// EXPANDER UTILS

static inline bool	single_dollar(char *input_at_i)
{
	return ((!input_at_i[1]
			|| input_at_i[1] == ' '
			|| input_at_i[1] == '\"'));
}

size_t				exit_status_size(void);
size_t				expand_size(char *input, size_t *i, t_data *data);
int					expanded_size(char *input, t_data *data);

void				init_vars(size_t *i, size_t *size, bool *in_quotes, bool *in_dquotes);
char				*expander(char *input, t_data *data);

// PARSER UTILS
t_operator			get_operator(char *operator);
size_t				get_argc(char **parsed);
size_t				get_token_len(char *input_at_i);
size_t				get_nr_statements(char *input);

t_statement			*parser(char *input);

void				print_operator(t_operator operator);

void				panic(t_data *data, char *error_msg, int exit_status);

#endif


// void	p_lstclear(t_statement **head)
// {
// 	t_statement	*temp;
// 	t_statement	*next_node;

// 	if (!head)
// 		return ;
// 	temp = *head;
// 	while (temp != NULL)
// 	{
// 		next_node = temp->next;
// 		free_matrix(temp->argv);
// 		free(temp);
// 		temp = next_node;
// 	}
// 	*head = NULL;
// }

size_t	p_lstsize(t_statement *head)
{
	t_statement	*temp;
	size_t		size;

	temp = head;
	size = 0;
	while (temp != NULL)
	{
		temp = temp->next;
		size += 1;
	}
	return (size);
}

t_statement	*p_new_node(int argc)
{
	t_statement	*new_node;

	new_node = malloc(sizeof(t_statement));
	new_node->argc = argc;
	new_node->argv = malloc((argc + 1) * sizeof(char *));
	new_node->operator = NONE;
	new_node->next = NULL;
	return (new_node);
}

//!invalid_syntax.c    
bool	unexpected_token(char token)
{
	ft_putstr_fd(UNEXPECTED_TOKEN, STDERR_FILENO);
	ft_putchar_fd(token, STDERR_FILENO);
	ft_putendl_fd("'", STDERR_FILENO);
	return (true);
}

bool	has_operator(char *input)
{
	size_t	i;

	i = 0;
	while (input[i])
	{
		if (is_onstr(OPERATORS, input[i]))
			return (true);
		i += 1;
	}
	return (false);
}

bool	invalid_syntax_on_operator(char *input)
{
	size_t	i;
	bool	in_quotes;

	i = 0;
	in_quotes = false;
	while (has_operator(&input[i]))
	{
		if (is_onstr(QUOTES, input[i]))
			in_quotes = !in_quotes;
		if (is_onstr(OPERATORS, input[i]) && !in_quotes)
		{
			if (input[i] == input[i + 1])
				i += 2;
			else
				i += 1;
			if (input[i] == ' ')
			{
				while (input[i] && input[i] == ' ')
					i += 1;
				if (is_onstr(OPERATORS, input[i]))
					return (unexpected_token(input[i]));
			}
			if (is_onstr(OPERATORS, input[i]))
				return (unexpected_token(input[i]));
		}
		i += 1;
	}
	return (false);
}

bool	invalid_syntax2(char *input)
{
	size_t	i;
	bool	in_quotes;

	i = 0;
	in_quotes = false;
	while (input[i])
	{
		if (is_onstr(QUOTES, input[i]))
			in_quotes = !in_quotes;
		if (((input[i] == '>' && input[i + 1] == '<')
				|| (input[i] == '<' && input[i + 1] == '>')
				|| (input[i] == '|' && input[i + 1] == '|')) && !in_quotes)
			return (unexpected_token(input[i + 1]));
		else if ((input[i] == '{' || input[i] == '}'
				|| input[i] == '(' || input[i] == ')'
				|| input[i] == '[' || input[i] == ']'
				|| input[i] == ';' || input[i] == '&' || input[i] == '*')
			&& !in_quotes)
			return (unexpected_token(input[i]));
		i += 1;
	}
	return (false);
}

bool	invalid_syntax(char *input)
{
	if (input[0] == '|')
		return (unexpected_token('|'));
	if (input[ft_strlen(input) - 1] == '|')
	{
		ft_putendl_fd(NO_PIPE_PROMPT, STDERR_FILENO);
		return (true);
	}
	if (is_onstr(REDIRECTS, input[ft_strlen(input) - 1]))
	{
		ft_putendl_fd(SYTX_ERR_RDR, STDERR_FILENO);
		return (true);
	}
	return (false);
}


//!parser_utils.c 

t_operator	get_operator(char *operator)
{
	t_operator	op;

	if (!operator)
		op = NONE;
	else if (streq(operator, "|"))
		op = PIPE;
	else if (streq(operator, ">>"))
		op = RDR_OUT_APPEND;
	else if (streq(operator, ">"))
		op = RDR_OUT_REPLACE;
	else if (streq(operator, "<<"))
		op = RDR_INPUT_UNTIL;
	else if (streq(operator, "<"))
		op = RDR_INPUT;
	else
		op = NONE;
	free(operator);
	return (op);
}

size_t	get_argc(char **parsed)
{
	size_t	i;

	i = 0;
	while (parsed[i])
		i += 1;
	return (i);
}

size_t	get_token_len(char *input_at_i)
{
	size_t	i;
	char	quotes;

	i = 0;
	if (is_onstr(OPERATORS, input_at_i[i]))
	{
		if (input_at_i[i] == input_at_i[i + 1])
			return (2);
		return (1);
	}
	while (input_at_i[i]
		&& !is_spaces(input_at_i[i])
		&& !is_onstr(OPERATORS, input_at_i[i]))
	{
		if (is_onstr(QUOTES, input_at_i[i]))
		{
			quotes = input_at_i[i++];
			while (input_at_i[i] && input_at_i[i] != quotes)
				i += 1;
		}
		i += 1;
	}
	return (i);
}

size_t	get_nr_statements(char *input)
{
	size_t	count;
	bool	flag;
	bool	quotes;

	count = 0;
	flag = false;
	quotes = false;
	while (*input)
	{
		if (is_onstr(OPERATORS, *input))
			count += 1;
		if (is_onstr(QUOTES, *input) && *input == *(input + 1))
			input += 1;
		else if (is_onstr(QUOTES, *input))
			quotes = !quotes;
		if (*input != ' ' && !is_onstr(OPERATORS, *input) && !flag && !quotes)
		{
			flag = true;
			count += 1;
		}
		else if (*input == ' ' || is_onstr(OPERATORS, *input))
			flag = false;
		input += 1;
	}
	return (count);
}

//!remove_quotes.c  

int	unclosed_quotes(char *str)
{
	char	last_opened;

	last_opened = 0;
	while (*str && !last_opened)
	{
		if (*str == '\'' || *str == '"')
			last_opened = *str;
		str++;
	}
	while (*str && last_opened)
	{
		if (*str && *str == last_opened)
			last_opened = 0;
		str++;
	}
	if (*str)
		return (unclosed_quotes(str));
	else if (!last_opened)
		return (0);
	else
		return (1);
}

static size_t	remove_quotes_size(char	*parsed)
{
	size_t	i;
	size_t	size;
	char	quotes;

	i = 0;
	size = 0;
	while (parsed[i])
	{
		while (parsed[i] && !is_onstr(QUOTES, parsed[i]))
		{
			i += 1;
			size += 1;
		}
		if (!parsed[i])
			break ;
		quotes = parsed[i++];
		while (parsed[i] && parsed[i] != quotes)
		{
			i += 1;
			size += 1;
		}
		quotes = 0;
	}
	return (size);
}

char	*remove_quotes(char	*parsed)
{
	size_t	i;
	size_t	j;
	char	quotes;
	char	*unquoted_parsed;

	i = 0;
	j = 0;
	quotes = '\0';
	unquoted_parsed = malloc((remove_quotes_size(parsed) + 1) * sizeof(char));
	while (parsed[i])
	{
		while (parsed[i] && !is_onstr(QUOTES, parsed[i]))
			unquoted_parsed[j++] = parsed[i++];
		if (!parsed[i])
			break ;
		quotes = parsed[i];
		i += 1;
		while (parsed[i] && parsed[i] != quotes)
			unquoted_parsed[j++] = parsed[i++];
		quotes = '\0';
	}
	unquoted_parsed[j] = '\0';
	free(parsed);
	return (unquoted_parsed);
}

//!parser.c 
extern long long	g_exit_status;

char	**parse_input(char *input)
{
	size_t		i;
	size_t		k;
	char		**parsed;
	size_t		len;
	size_t		j;

	i = 0;
	k = 0;
	parsed = malloc((get_nr_statements(input) + 1) * sizeof(char *));
	while (input[i])
	{
		len = get_token_len(&input[i]);
		if (!len)
		{
			i += 1;
			continue ;
		}
		parsed[k] = malloc((len + 1) * sizeof(char));
		j = 0;
		while (input[i] && j < len)
			parsed[k][j++] = input[i++];
		parsed[k++][j] = '\0';
	}
	parsed[k] = NULL;
	return (parsed);
}

t_statement	*parser(char *input)
{
	char		**parsed;
	t_statement	*temp;
	t_statement	*head;
	size_t		idx[2];

	parsed = parse_input(input);
	free(input);
	temp = p_new_node(get_argc(&parsed[0]));
	head = temp;
	idx[0] = 0;
	while (parsed[idx[0]])
	{
		idx[1] = 0;
		while (parsed[idx[0]] && !is_onstr(OPERATORS, parsed[idx[0]][0]))
			temp->argv[idx[1]++] = remove_quotes(parsed[idx[0]++]);
		temp->argv[idx[1]] = NULL;
		if (!parsed[idx[0]])
			break ;
		temp->operator = get_operator(parsed[idx[0]++]);
		temp->next = p_new_node(get_argc(&parsed[idx[1]]));
		temp = temp->next;
	}
	temp->next = NULL;
	// free(parsed);
	return (head);
}

// Helper function to print the parsed statement list

// void p_lstclear(t_statement **head) {
//     if (!head || !*head)
//         return;

//     t_statement *current = *head;
//     t_statement *next = NULL;

//     while (current != NULL) {
//         next = current->next;
        
//         // Free argv array if dynamically allocated
//         if (current->argv) {
//             for (int i = 0; i < current->argc; ++i) {
//                 free(current->argv[i]);
//             }
//             free(current->argv);
//         }
        
//         free(current);
//         current = next;
//     }

//     *head = NULL;
// }

void print_statement_list(t_statement *head) {
    t_statement *current = head;
    while (current != NULL) {
        printf("Arguments (%d): ", current->argc);
        for (int i = 0; i < current->argc; ++i) {
            printf("%s ", current->argv[i]);
        }
        printf("\nOperator: ");
        switch (current->operator) {
            case NONE:
                printf("NONE\n");
                break;
            case RDR_OUT_REPLACE:
                printf("RDR_OUT_REPLACE\n");
                break;
            case RDR_OUT_APPEND:
                printf("RDR_OUT_APPEND\n");
                break;
            case RDR_INPUT:
                printf("RDR_INPUT\n");
                break;
            case RDR_INPUT_UNTIL:
                printf("RDR_INPUT_UNTIL\n");
                break;
            case PIPE:
                printf("PIPE\n");
                break;
            default:
                printf("UNKNOWN\n");
                break;
        }
        current = current->next;
    }
}

int main() {
    // Example input command line
    char input[] = "ls -l | grep .c > output.txt";

    // Parse the input to create a statement list
    t_statement *statement_list = parser(input);

    // Print the parsed statement list
    print_statement_list(statement_list);

    // Free the allocated memory for the statement list
    // p_lstclear(&statement_list);

    return 0;
}
