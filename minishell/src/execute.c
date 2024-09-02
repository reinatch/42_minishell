

#include "../include/minishell.h"

int ft_strcmp(const char *s1, const char *s2)
{
	int i;

	i = 0;
	while (s1[i] && s2[i])
	{
		if (s1[i] != s2[i])
			break ;
		i++;
	}
	return (s1[i] - s2[i]);
}

void free_list(Token *cmd)
{
	Token *remove;

	while (cmd != NULL)
	{
		remove = cmd;
		cmd = cmd->next;
		free(remove);
	}
}

void close_fds(int *fd)
{
	if (fd[0] != 0)
		close(fd[0]);
	if (fd[1] != 1)
		close(fd[1]);
}

char *find_path(char **envp)
{
	int i;
	char *v_return;

	i = 0;
	v_return = NULL;
	while (envp[i] != NULL)
	{
		if (!ft_strncmp(envp[i], "PATH", 4))
		{
			v_return = ft_strdup(envp[i]);
			return v_return;
		}
		i++;
	}
	return v_return;
}

// char *ft_prompt()
// {
// 	char *v_return;

// 	v_return = readline("minishell>: ");
// 	if (ft_strlen(v_return) == 0)
// 		return (free(v_return), printf("\n"), ft_prompt());
// 	add_history(v_return);
// 	return (v_return);
// }

int there_is_pipes(Token *cmds)
{
	// int i;

	// i = 0;
	while (cmds != NULL)
	{
		if (cmds->pipe_to_next_token)
			return 1;
		cmds = cmds->next;
	}
	return 0;
}

void free_a_arrays(char **str)
{
	int i;

	i = 0;
	while (str[i])
	{
		free(str[i]);
		i++;
	}
	free(str);
}

char *join_cmd_with_path(char *cmd, char *path)
{
	char *full_path;
	char *half_path;

	half_path = NULL;
	half_path = ft_strjoin(path, "/");
	if (!half_path)
		return NULL;
	full_path = ft_strjoin(half_path, cmd);
	if (half_path)
		free(half_path);
	return (full_path);
}

char *command_path(char **to_execute, char **env)
{
	char *path_env;
	char *path;
	char **full_path;
	char *half_path;
	int i;
	
	half_path = NULL;
	path_env = find_path(env);
	full_path = ft_split(path_env + 5, ':');
	free(path_env);
	i = 0;
	while (full_path[i])
	{
		path = join_cmd_with_path(to_execute[0], full_path[i]);
		free(half_path);
		if (access(path, F_OK | X_OK) == 0)
			return (free_a_arrays(full_path), path);
		free(path);
		i++;
	}
	return NULL;
}

void child_process(Token *cmds, char **env)
{
	int fd_in;
	int fd_out;

	fd_in = 0;
	fd_out = 1;
	if (cmds->input_file != NULL)
	{
		fd_in = open(cmds->input_file, O_RDONLY);
		dup2(fd_in, 0);
	}
	if (cmds->output_file)
	{
		if (cmds->append)
			fd_out = open(cmds->output_file, O_WRONLY, O_APPEND, O_CREAT);
		else
			fd_out = open(cmds->output_file, O_WRONLY, O_TRUNC, O_CREAT);
		dup2(fd_out, 1);
	}
	execve(command_path(cmds->to_execute, env), cmds->to_execute, env);
	if (cmds->input_file)
		close (fd_in);
	if (cmds->output_file)
		close (fd_out);
	exit(127);
}

void ft_cd_builtin(Token *cmds)
{
	char *error;

	if (ft_strlen(cmds->to_execute[0]) != 2 || cmds->to_execute[1] == NULL)
		write(1, "\n", 1);
	else if (cmds->to_execute[2] != NULL)
		printf("bash : cd: too many arguments\n");
	else if (chdir(cmds->to_execute[1]) == -1)
	{
		error = ft_strjoin("minishell: cd: ", cmds->to_execute[1]);
		perror(error);
		free(error);
	}
}

void ft_pwd_builtin(Token *cmd)
{
	char *cwd;
	size_t size;
	int *fd;

	size = 2;
	fd = malloc(sizeof(int) * 2);
	cwd = malloc(sizeof(char) * size);
	while (getcwd(cwd, size) == NULL)
	{
		if (errno == ERANGE)
		{
			size *= 2;
			free(cwd);
			cwd = malloc(sizeof(char) * size);
		}
		else
			perror("getcwd");
	}
	decide_in_and_out(cmd, &fd);
	write_in_fd(cwd, fd[1]);
	write_in_fd("\n", fd[1]);
	close_fds(fd);
	free(fd);
	free(cwd);
}

int is_n_flag(char **to_execute)
{
	int i;
	
	i = 0;
	if (!to_execute[1])
		return 0;
	if (to_execute[1][i++] != '-')
		return 0;
	while (to_execute[1][i])
	{
		if (to_execute[1][i++] != 'n')
			return 0;
	}
	return 1;
}

void decide_in_and_out(Token *cmd, int **fds)
{
	(*fds)[0] = 0;
	(*fds)[1] = 1;
	if (cmd->input_file)
		*(fds[0]) = open(cmd->input_file, O_RDONLY);
	if (cmd->output_file)
	{
		if (cmd->append)
			(*fds)[1] = open(cmd->output_file, O_WRONLY | O_APPEND | O_CREAT);
		else
			(*fds)[1] = open(cmd->output_file, O_WRONLY | O_TRUNC | O_CREAT);
	}
}

int roam_while_isnt_assigned_char(char *str, char c)
{
	int i;

	i = 0;
	while (str[i] != '\0' && str[i] != c)
		i++;
	if (str[i] == '\0')
		return -1;
	return i;
}

// int is_even_quotes(char *str)
// {
// 	int i;
// 	int how_far_went;
// 	int words;
//
// 	i = 0;
// 	words = 1;
// 	while (str[i] != '\0')
// 	{
// 		how_far_went = 0;
// 		if (str[i] == '\"' || str[i] == '\'')
// 		{
// 			if (str[i] == '\"')
// 				how_far_went += roam_while_isnt_assigned_char(++i + str, '\"');
// 			else if (str[i] == '\'')
// 				how_far_went += roam_while_isnt_assigned_char(++i + str, '\'');
// 			if (how_far_went == -1)
// 				return 0;
// 			words++;
// 			i += how_far_went;
// 		}
// 		i++;
// 	}
// 	return words;
// }

int fulfil_word(char *str, char **to_execute, char c)
{
	int word_len;
	int i;

	word_len = 0;
	while (str[word_len] != '\0' && str[word_len] != c)
		word_len++;
	(*to_execute) = malloc(sizeof(char) * (word_len + 1));
	(*to_execute)[word_len] = '\0';
	i = 0;
	while (i < word_len)
	{
		(*to_execute)[i] = str[i];
		i++;
	}
	return i + 1;
}

void separate_by_quote(char *str, int *i, char **ret, char c, int *j)
{
	int i_cpy;
	int k;

	(*i)++;
	i_cpy = (*i);
	k = 0;
	while (str[i_cpy] != c)
	{
		printf("string na posicao %d tem caractere %c\n", i_cpy, str[i_cpy]);
		i_cpy++;
	}
	(*ret) = malloc(sizeof(char) * (i_cpy - (*i) + 1));
	(*ret)[i_cpy - (*i)] = '\0';
	while (str[(*i)] != '\0' && str[(*i)] != c)
	{
		(*ret)[k] = str[(*i)];
		k++;
		(*i)++;
	}
	if (str[(*i)] != '\0')
		(*i)++;
	(*j)++;
}

void separate_by_space(char *str, int *i, char **ret, int *j)
{
	int i_cpy;
	int counter;

	if (str[*i] == '\0')
		return ;
	while (str[*i] == ' ')
		(*i)++;
	i_cpy = (*i);
	while (str[i_cpy] != '\0' && str[i_cpy] != '\'' && str[i_cpy] != '"' && str[i_cpy] != ' ')
		i_cpy++;
	if (i_cpy != (*i))
	{
		counter = 0;
		(*ret) = malloc(sizeof(char) * (i_cpy - (*i) + 1));
		while  ((*i) < i_cpy)
			(*ret)[counter++] = str[(*i)++];
		(*j)++;
	}
}

// void take_quote_from_beggining(char ***ret)
// {
// 	char *copy;
// 	int j;
// 	int len;
// 	int i;
//
// 	j = 0;
// 	while ((*ret)[j])
// 	{
// 		if ((*ret)[j][0] == '\'' || (*ret)[j][0] == '"')
// 		{
// 			i = 1;
// 			len = ft_strlen((*ret)[j]);
// 			copy = ft_strdup((*ret)[j]);
// 			free((*ret)[j]);
// 			(*ret)[j] = malloc(sizeof(char) * (len));
// 			while(i <= len)
// 				(*ret)[j][i++] = copy[j - 1];
// 		}
// 		j++;
// 	}
// }

char **separate_command(char *str, int n_args)
{
	char **ret;
	int i;
	int j;

	j = 0;
	i = 0;
	printf("O numero de argumentos e' %d\n", n_args);
	ret = malloc(sizeof(char *) * (n_args + 1));
	while (str[i] != '\0')
	{
		if (str[i] == '\'' || str[i] == '"')
			separate_by_quote(str, &i, &(ret[j]), str[i], &j);
		else
			separate_by_space(str, &i, &(ret[j]), &j);
	}
	if (ret[n_args] != NULL)
		free(ret[n_args]);
	ret[n_args] = NULL;
	// take_quote_from_beggining(&ret);
	return (ret);
}

int where_is_last_n_flag(char **to_execute)
{
	int i;
	int j;
	int last_nflag_position;

	i = 1;
	last_nflag_position = 1;
	while (to_execute[i] != NULL)
	{
		j = 0;
		if (to_execute[i][j] == '-')
		{
			j++;
			while (to_execute[i][j] == 'n')
				j++;
			if (to_execute[i][j] == '\0')
				last_nflag_position = i;
			else
				break ;
		}
		i++;
	}
	return last_nflag_position;
}

// here, I need to free everything and close the file directorys, if needed
void print_echo(int n_flag, char **to_execute, int *fds)
{
	int i;

	if (n_flag)
		i = where_is_last_n_flag(to_execute) + 1;
	else
		i = 1;
	while (to_execute[i])
	{
		write_in_fd(to_execute[i++], fds[1]);
		if (to_execute[i] != NULL)
			write_in_fd(" ", fds[1]);
	}
	if (!n_flag)
		write_in_fd("\n", fds[1]);
	close_fds(fds);
}

void ft_echo_builtin(Token *cmd) 
{
	int *fds;
	char *str_error;
	// int i;

	// i = 0;
	fds = malloc(sizeof(int) * 2);
	decide_in_and_out(cmd, &fds);
	if (ft_strlen(cmd->to_execute[0]) != 4)
	{
		str_error = ft_strjoin(cmd->to_execute[0], ": command not found");
		write_in_fd(str_error, fds[1]);
		write(fds[1], "\n", 1);
		free(str_error);
	}
	else
		print_echo(is_n_flag(cmd->to_execute), cmd->to_execute, fds);
}

void ft_env_builtin(Token *cmd, char **env)
{
	int fd;
	int i;

	i = 0;
	fd = 1;
	if (cmd->output_file)
	{
		if (cmd->append)
			fd = open(cmd->output_file, O_WRONLY | O_APPEND | O_CREAT);
		else
			fd = open(cmd->output_file, O_WRONLY | O_TRUNC | O_CREAT);
	}
	while (env[i])
	{
		write_in_fd(env[i], fd);
		write_in_fd("\n", fd);
		i++;
	}
	if (cmd->output_file)
		close (fd);
}

void write_in_fd(char *str, int fd)
{
	int i;

	i = 0;
	while (str[i])
		write(fd, &(str[i++]), 1);
}

int number_of_strs(char **env)
{
	int i;

	i = 0;
	while (env[i])
		i++;
	return i;
}

void sort_pos_of_envp(char **envp, int **pos_sorted, int len)
{
	int i;
	int j;
	int temp;

	i = 0;
	while (i + 1 < len)
	{
		j = i + 1;
		while (j < len)
		{
			if (ft_strcmp(envp[(*pos_sorted)[i]], envp[(*pos_sorted)[j]]) > 0)
			{
				temp = *(pos_sorted[i]);
				*(pos_sorted[i]) = *(pos_sorted[j]);
				*(pos_sorted[j]) = temp;
				i = -1;
				break;
			}
			j++;
		}
		i++;
	}
}

void ft_env_sorted(char **envp, int fd)
{
	int *pos_sorted;
	int n_strs;
	int i;
	
	if (!envp)
		return ;
	n_strs = number_of_strs(envp);
	pos_sorted = malloc(sizeof(int) * n_strs);
	i = -1;
	while (envp[++i])
		pos_sorted[i] = i;
	sort_pos_of_envp(envp, &pos_sorted ,n_strs);
	i = 0;
	while (i < n_strs)
	{
		write(fd, "declare -x ", 11);
		write_in_fd(envp[pos_sorted[i++]], fd);
		write(fd, "\n", 1);
	}
	if (fd != 1)
		close(fd);
}

void print_export_error(char *str, int fd)
{
	int i;

	i = 0;
	write(fd, "minishell: export: `", 20);
	while (str[i] != '\0')
		write(fd, &(str[i++]), 1);
	write(fd, "': not a valid identifier", 25);
	if (fd != 1)
		close(fd);
}

int valid_export_args(char **to_execute, int fd)
{
	int i;
	int j;

	i = 1;
	while (to_execute[i])
	{
		j = 0;
		while (to_execute[i][j])
		{
			if (!(ft_isalnum(to_execute[i][j]) || to_execute[i][j] == '='))
			{
				print_export_error(to_execute[i], fd);
				return 0;
			}
			j++;
		}
		i++;
	}
	return 1;
}

int exec_has_repeated_var(char *exec, char *env_var)
{
	int i;

	i = 0;
	while(exec[i] && env_var[i])
	{
		if (exec[i] != env_var[i])
			return 0;
		else if (exec[i] == '=')
			break ;
	}
	return 1;
}

char **ft_array_strdup(char **envp)
{
	int n_strs;
	char **new_array;

	n_strs = 0;
	while (envp[n_strs])
		n_strs++;
	new_array = malloc(sizeof(char *) * (n_strs + 1));
	n_strs = 0;
	while (envp[n_strs])
	{
		new_array[n_strs] = ft_strdup(envp[n_strs]);
		n_strs++;
	}
	new_array[n_strs] = NULL;
	return new_array;
}

char **ft_envp_with_new_str(char **envp, int n_strs, char **to_execute)
{
	char **new_envp;
	int i;

	i = 0;
	if (!to_execute[0])
		return envp;
	while (envp[i]) 
	{
		if (exec_has_repeated_var(to_execute[0], envp[i]))
		{
			envp[i] = ft_strdup(to_execute[0]);
			return (ft_envp_with_new_str(envp, n_strs, ++to_execute));
		}
		i++;
	}
	new_envp = malloc(sizeof(char *) * (n_strs + 2));
	new_envp = ft_array_strdup(envp);
	new_envp[n_strs] = ft_strdup(to_execute[0]);
	new_envp[n_strs + 1] = NULL;
	free_a_arrays(envp);
	return ft_envp_with_new_str(new_envp, n_strs + 1, ++to_execute);
}

// the export command, without any other parameter, prints all the environment
// variables alphabetically sorteds.
//
// FALTA ME VERIFICAR SE O ENVIRONMENT JA TEM A VARIAVEL QUE QUERO COLOCAR
char **ft_export_builtin(Token *cmd, char **envp)
{
	int n_strs;
	char **new_envp;
	int fd;

	fd = 1;
	new_envp = NULL;
	if (cmd->output_file)
		fd = open(cmd->output_file, O_RDONLY);
	if (cmd->to_execute[1] == NULL)
		ft_env_sorted(envp, fd);
	else
	{
		if (!valid_export_args(cmd->to_execute, fd))
			return envp;
		n_strs = number_of_strs(envp);
		new_envp = ft_envp_with_new_str(envp, n_strs, cmd->to_execute);
		free_a_arrays(envp);
	}
	if (cmd->output_file)
		close(fd);
	return new_envp;
}

char **ft_unset_builtin(char **to_execute, char **envp)
{
	char **new_envp;
	int i;
	int j;
	int k;

	if (!to_execute || !to_execute[1])
		return envp;
	i = 0;
	k = 0;
	new_envp = malloc(sizeof(char *) * (number_of_strs(envp) + 1));
	while(envp[i])
	{
		j = 1;
		while (to_execute[j])
		{
			if (exec_has_repeated_var(to_execute[j], envp[i]))
				break ;
			else if (!to_execute[j + 1])
				new_envp[k++] = ft_strdup(to_execute[j]);
			j++;
		}
		i++;
	}
	free_a_arrays(envp);
	return (new_envp);
}

void simple_command(Token *cmds, char **envp)
{
	int pid;

	pid = fork();
	if (pid == 0)
		child_process(cmds, envp);
	waitpid(pid, NULL, 0 );
}



void no_pipes(Token *cmds, char **envp)
{
	char **return_of_env;

	if (ft_strncmp(cmds->to_execute[0], "cd", 2) == 0)
		ft_cd_builtin(cmds);
	else if (ft_strncmp(cmds->to_execute[0], "pwd", 3) == 0)
		ft_pwd_builtin(cmds);
	else if (ft_strncmp(cmds->to_execute[0], "export", 6) == 0)
	{
		return_of_env = ft_export_builtin(cmds, envp);
		envp = return_of_env;
	}
	else if (ft_strncmp(cmds->to_execute[0], "unset", 5) == 0)
	{
		return_of_env = ft_unset_builtin(cmds->to_execute, envp);
		envp = return_of_env;
	}
	else if (ft_strncmp(cmds->to_execute[0], "env", 3) == 0)
		ft_env_builtin(cmds, envp);
	else if (ft_strncmp(cmds->to_execute[0], "echo", 4) == 0)
		ft_echo_builtin(cmds);
	else
		simple_command(cmds, envp);
}

// void ft_pwd_with_pipe(Token *cmd, int *pipe_fds) // has 26 lines
// {
// 	char *cwd;
// 	size_t size;
// 	int *fd;
//
// 	size = 2;
// 	fd = malloc(sizeof(int) * 2);
// 	cwd = malloc(sizeof(char) * size);
// 	while (getcwd(cwd, size) == NULL)
// 	{
// 		if (errno == ERANGE)
// 		{
// 			size *= 2;
// 			free(cwd);
// 			cwd = malloc(sizeof(char) * size);
// 		}
// 		else
// 			perror("getcwd");
// 	}
// 	decide_in_and_out(cmd, &fd);
// 	if (fd[1] == 1)
// 		fd[1] = pipe_fds[0];
// 	write_in_fd(cwd, fd[1]);
// 	write_in_fd("\n", fd[1]);
// 	close_fds(fd);
// 	free(fd);
// 	free(cwd);
// }
//
// char **ft_export_with_pipe(Token *cmd, char **envp, int* pipe_fds)
// {
// 	int n_strs;
// 	char **new_envp;
// 	int fd;
//
// 	fd = 1;
// 	new_envp = NULL;
// 	if (cmd->output_file)
// 		fd = open(cmd->output_file, O_RDONLY);
// 	if (cmd->to_execute[1] == NULL && cmd->output_file)
// 		ft_env_sorted(envp, fd);
// 	else if (cmd->to_execute[1] == NULL && !cmd->output_file)
// 		ft_env_sorted(envp, pipe_fds[0]);
// 	else
// 	{
// 		if (!valid_export_args(cmd->to_execute, fd))
// 			return envp;
// 		n_strs = number_of_strs(envp);
// 		new_envp = ft_envp_with_new_str(envp, n_strs, cmd->to_execute);
// 		free_a_arrays(envp);
// 	}
// 	if (cmd->output_file)
// 		close(fd);
// 	return new_envp;
// }
//
// void ft_env_with_pipe(Token *cmd, char **env, int *pipe_fds)
// {
// 	int fd;
// 	int i;
//
// 	i = 0;
// 	fd = 1;
// 	if (cmd->output_file)
// 	{
// 		if (cmd->append)
// 			fd = open(cmd->output_file, O_WRONLY | O_APPEND | O_CREAT);
// 		else
// 			fd = open(cmd->output_file, O_WRONLY | O_TRUNC | O_CREAT);
// 	}
// 	while (env[i])
// 	{
// 		write_in_fd(env[i], fd);
// 		write_in_fd("\n", fd);
// 		i++;
// 	}
// 	if (cmd->output_file)
// 		close (fd);
// }
//
// void execution_of_pipes(int *pipe_fds, Token *cmds, char **envp)
// {
// 	char **return_of_env;
//
// 	if (ft_strncmp(cmds->to_execute[0], "pwd", 3) == 0)
// 		ft_pwd_with_pipe(cmds, pipe_fds);
// 	else if (ft_strncmp(cmds->to_execute[0], "export", 6) == 0)
// 	{
// 		return_of_env = ft_export_with_pipe(cmds, envp, pipe_fds);
// 		envp = return_of_env;
// 	}
// 	else if (ft_strncmp(cmds->to_execute[0], "env", 3) == 0)
// 		ft_env_with_pipe(cmds, envp, pipe_fds);
// 	else if (ft_strncmp(cmds->to_execute[0], "echo", 4) == 0)
// 		ft_echo_with_pipe(cmds);
// 	else if (is_a_builtin(cmds))
// 		no_pipes(cmds, envp);
// 	else
// 		simple_command_with_pipe(cmds, envp);
// }
//
// void with_pipes(Token *cmds, char **envp)
// {
// 	int pipe_fds[2];
// 	int fd_in;
//
// 	pipe(pipe_fds);
// 	while (cmds)
// 	{
// 		dup2(1, pipe_fds[0]);
// 		execution_of_pipes(pipe_fds, cmds, envp);
// 		dup2(0, pipe_fds[1]);
// 		cmds = cmds->next;
// 	}
// 	restore_fds(pipe_fds);
// }

void after_receiving_cmds(Token *cmds, char **envp)
{
	// if (there_is_pipes(cmds))
	//	 with_pipes(cmds, envp);
	// else
		no_pipes(cmds, envp);
}

// int main(int ac, char **av, char **envp)
// {
// 	// char *path;
// 	// char *input;
// 	Token *cmds;
// 	char **my_env;
// 	(void)ac; (void)av;
// 	my_env = ft_array_strdup(envp);
// 		cmds = calloc(1, sizeof(struct Token));
// 	cmds->pipe_to_next_token = 0;
// 	cmds->to_execute = malloc(sizeof(char *) * 8);
// 	cmds->to_execute[0] = ft_strdup("cat");
// 	cmds->to_execute[1] = ft_strdup("asd.txt");
// 	// cmds->to_execute[2] = ft_strdup("-n");
// 	// cmds->to_execute[3] = ft_strdup("-nnnnn-");
// 	// cmds->to_execute[4] = ft_strdup("Hello World");
// 	// cmds->to_execute[5] = ft_strdup("; Hello again");
// 	// cmds->to_execute[6] = ft_strdup("\n\"Now, hello with double ' \"");
// 	cmds->to_execute[2] = NULL;
// 	cmds->append = 1;
// 	cmds->input_file = NULL;
// 	cmds->output_file = "asd.txt";
// 	// cmds->next = calloc(1, sizeof(struct Token));
// 	// cmds->next->to_execute = malloc(sizeof (char *) * 3);
// 	// cmds->next->to_execute[0] = ft_strdup("grep");
// 	// cmds->next->to_execute[1] = ft_strdup("hel");
// 	// cmds->next->to_execute[2] = NULL;
// 	// cmds->next->output_file = NULL;
// 	// cmds->next->input_file = NULL;
// 	// cmds->next->append = 0;
// 	// cmds->next->pipe_to_next_token = 0;
// 	// cmds->next->next = NULL;
// 	after_receiving_cmds(cmds, my_env);
// 	free_a_arrays(cmds->to_execute);
// 	// free_a_arrays(cmds->next->to_execute);
// 	free_list(cmds);
// }



// int main(int ac, char **av, char **envp)
// {
// 	char *path;
// 	char *input;
// 	Token *cmds;
// 	char **my_env;
//
// 	my_env = copy_env(envp);
// 	while (1)
// 	{
// 		input = ft_prompt();
// 		cmds = parsing(input);
// 		after_receiving_cmds(cmds, my_env);
// 		free(input);
// 		free_list(cmds); // if needed
// 	}
// }
