#include "minishell.h"
#include "libft_printf/libft.h"

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

char *ft_prompt()
{
	char *v_return;

	v_return = readline("minishell>: ");
	if (ft_strlen(v_return) == 0)
		return (free(v_return), printf("\n"), ft_prompt());
	add_history(v_return);
	return (v_return);
}

int there_is_pipes(Token *cmds)
{
	int i;

	i = 0;
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

void child_process(char **to_execute, Token *cmds, char **env)
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
	execve(command_path(to_execute, env), to_execute, env);
	if (cmds->input_file)
		close (fd_in);
	if (cmds->output_file)
		close (fd_out);
	exit(127);
}

void ft_cd_builtin(Token *cmds, char **to_execute)
{
	char *error;

	if (chdir(cmds->value) == -1)
	{
		error = ft_strjoin("minishell: cd: ", to_execute[1]);
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
			*(fds[1]) = open(cmd->output_file, O_WRONLY | O_APPEND | O_CREAT);
		else
			*(fds[1]) = open(cmd->output_file, O_WRONLY, O_TRUNC, O_CREAT);
	}
}

void ft_echo_builtin(Token *cmd, char **to_execute) 
{
	int i;
	int flag;
	int *fds;
	int j;

	flag = 0;
	i = 1;
	if (to_execute[i++] && is_n_flag(to_execute))
		flag = 1;
	decide_in_and_out(cmd, &fds);
	if (fds[0] != 0)
	{
		if (!flag)
			write(1, "\n", 1);
		return ;
	}
	while (to_execute[i])
	{
		j = 0;
		while (to_execute[i][j])
			write(fds[1], &(to_execute[i][j++]), 1);
		i++;
	}
	close_fds(fds);
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
			if (ft_strcmp(envp[*(pos_sorted[i])], envp[*(pos_sorted[j])]) > 0)
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
	while (envp[i++])
		pos_sorted[i] = i;
	sort_pos_of_envp(envp, &pos_sorted ,n_strs);
	i = 0;
	while (i < n_strs)
	{
		write(fd, "declare -x ", 11);
		write_in_fd(envp[pos_sorted[i++]], fd);
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
char **ft_export_builtin(Token *cmd, char **to_execute, char **envp)
{
	int n_strs;
	char **new_envp;
	int fd;

	fd = 1;
	if (cmd->output_file)
		fd = open(cmd->output_file, O_RDONLY);
	if (to_execute[1] == NULL)
		ft_env_sorted(envp, fd);
	else
	{
		if (!valid_export_args(to_execute, fd))
			return envp;
		n_strs = number_of_strs(envp);
		new_envp = ft_envp_with_new_str(envp, n_strs, to_execute);
		free_a_arrays(envp);
		envp = new_envp;
	}
	if (cmd->output_file)
		close(fd);
	return envp;
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

void no_pipes(Token *cmds, char **envp)
{
	int pid;
	char **to_execute;
	
	to_execute = ft_split(cmds->value, ' ');
	if (ft_strncmp(to_execute[0], "cd", 2) == 0)
		ft_cd_builtin(cmds, to_execute);
	else if (ft_strncmp(to_execute[0], "pwd", 3) == 0)
		ft_pwd_builtin(cmds);
	else if (ft_strncmp(to_execute[0], "export", 6) == 0)
		envp = ft_export_builtin(cmds, to_execute, envp);
	else if (ft_strncmp(to_execute[0], "unset", 5) == 0)
		envp = ft_unset_builtin(to_execute, envp);
	else if (ft_strncmp(to_execute[0], "env", 3) == 0)
		ft_env_builtin(cmds, envp);
	else if (ft_strncmp(to_execute[0], "echo", 4) == 0)
		ft_echo_builtin(cmds, to_execute);
	else
	{
		pid = fork();
		if (pid == 0)
				child_process(to_execute, cmds, envp);
		waitpid(pid, NULL, 0);
		free_a_arrays(to_execute);
	}
}

void with_pipes(Token *cmds, char **envp)
{
	int pid;
	int pipe_fds[2];

	while(cmds->pipe_to_next_token)
	{
		pipe(pipe_fds);
		pid = fork();
		if (pid == 0)
		{
			close(pipe_fds[0]);
			if (!cmds->input_file)
				dup2(pipe_fds[1], 1);
			no_pipes(cmds, envp);
		}
		if (!cmds->input_file)
			dup2(pipe_fds[0], 0);
		waitpid(pid, NULL, 0);
		cmds = cmds->next;
	}
	no_pipes(cmds, envp);
}

void after_receiving_cmds(Token *cmds, char **envp)
{
	if (there_is_pipes(cmds))
		with_pipes(cmds, envp);
	else
		no_pipes(cmds, envp);
}

int main(int ac, char **av, char **envp)
{
	// char *path;
	// char *input;
	Token *cmds;
	char **my_env;
	(void)ac; (void)av;
	my_env = ft_array_strdup(envp);
		cmds = calloc(1, sizeof(struct Token));
	cmds->pipe_to_next_token = 0;
	cmds->next = NULL;
	cmds->value = "env";
	cmds->append = 0;
	cmds->input_file = NULL;
	cmds->output_file = "teset.txt";
	after_receiving_cmds(cmds, my_env);
	free_list(cmds);
}

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
