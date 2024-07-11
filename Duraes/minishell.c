#include "minishell.h"
#include "libft_printf/libft.h"
#include <unistd.h>

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

char *command_path(char *cmd, char **env)
{
	char *path_env;
	char *path;
	char **full_path;
	char *half_path;
	int i;

	path_env = find_path(env);
	free(path_env);
	full_path = ft_split(path_env + 5, ':');
	i = 0;
	while (full_path[i])
	{
		half_path = ft_strjoin(full_path[i], "/");
		path = ft_strjoin(half_path, cmd);
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

	if (cmds->input_file)
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
	execve(command_path(cmds->value, env), to_execute, env);
	close (fd_in);
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

void ft_pwd_builtin()
{
	char *cwd;
	size_t size;

	size = 2;
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

void decide_in_and_out(Token *cmd, int *fd_in, int *fd_out)
{
	*fd_in = 0;
	*fd_out = 0;
	if (cmd->input_file)
		*fd_in = open(cmd->input_file, O_RDONLY);
	if (cmd->output_file)
	{
		if (cmd->append)
			*fd_out = open(cmd->output_file, O_WRONLY, O_APPEND, O_CREAT);
		else
			*fd_out = open(cmd->output_file, O_WRONLY, O_TRUNC, O_CREAT);
	}
}

void ft_echo_builtin(Token *cmd, char **to_execute) 
{
	int i;
	int flag;
	int fd_in;
	int fd_out;
	int j;

	flag = 0;
	i = 1;
	if (to_execute[i++] && is_n_flag(to_execute))
		flag = 1;
	decide_in_and_out(cmd, &fd_in, &fd_out);
	if (fd_in != 0)
	{
		if (!flag)
			write(1, "\n", 1);
		return ;
	}
	while (to_execute[i])
	{
		j = 0;
		while (to_execute[i][j])
			write(fd_out, &(to_execute[i][j++]), 1);
		i++;
	}
}

void ft_env_builtin(char **env)
{
	int i;

	i = 0;
	while (env[i])
		printf("%s\n", env[i]);
}

void no_pipes(Token *cmds, char **envp)
{
	int pid;
	char **to_execute;

	to_execute = ft_split(cmds->value, ' ');
	if (ft_strncmp(to_execute[0], "cd", 2) == 0)
		ft_cd_builtin(cmds, to_execute);
	else if (ft_strncmp(to_execute[0], "pwd", 3) == 0)
		ft_pwd_builtin();
	else if (ft_strncmp(to_execute[0], "export", 6) == 0)
		envp = ft_export_builtin(cmds, to_execute, envp);
	else if (ft_strncmp(to_execute[0], "unset", 5) == 0)
		envp = ft_unset_builtin(cmds, to_execute, envp);
	else if (ft_strncmp(to_execute[0], "env", 3) == 0)
		ft_env_builtin(envp);
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
	int fd_in;
	int fd_out;

	while(cmds->pipe_to_next_token)
	{
		decide_in_and_out(cmds, &fd_in, &fd_out);
		if (fd_in)
			
	}
}

void after_receiving_cmds(Token *cmds, char **envp)
{
	if (there_is_pipes(cmds))
		with_pipes(cmds, envp);
	else
		no_pipes(cmds, envp);
}

char **copy_env(char **envp)
{
	int i;
	int j;
	char **my_env;

	i = 0;
	while (envp[i])
		i++;
	my_env = malloc(sizeof(char *) * (i + 1));
	j = 0;
	while (j < i)
	{
		my_env[j] = ft_strdup(envp[j]);
		j++;
	}
	my_env[j] = NULL;
}

int main(int ac, char **av, char **envp)
{
	char *path;
	char *input;
	Token *cmds;
	char **my_env;

	my_env = copy_env(envp);
	while (1)
	{
		input = ft_prompt();
		cmds = parsing(input);
		after_receiving_cmds(cmds, my_env);
		free(input);
		free_list(cmds); // if needed
	}
}
