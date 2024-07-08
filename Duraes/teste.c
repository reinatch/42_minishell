#include "minishell.h"

char	*ft_strdup(const char *s) // esta na libft
{
	char	*string;
	int		i;

	i = 0;
	while (s[i] != '\0')
		i++;
	string = malloc(sizeof(char) * (i + 1));
	if (string == NULL)
		return (NULL);
	i = 0;
	while (s[i] != '\0')
	{
		string[i] = s[i];
		i++;
	}
	string[i] = '\0';
	return (string);
}

int	ft_strncmp(char *s1, char *s2, size_t n) // esta na libft
{
	size_t	i;
	int		comp;

	comp = 0;
	i = 0; 
	while ((s1[i] != '\0' || s2[i] != '\0') && i < n)
	{
		comp = (unsigned char)s1[i] - (unsigned char)s2[i];
		if (comp != 0)
		{
			return (comp);
		}
		i++;
	}
	return (comp);
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

	v_return = readline("minishell>: "); // falta adicionar o input ao historico
	return (v_return);
}

int there_is_pipes(Token *cmds)
{
	int i;

	i = 0;
	while (cmds != NULL)
	{
		if (cmds->type = TOKEN_PIPE)
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
			return (ft_free(full_path), path);
		free(path);
		i++;
	}
	return NULL;
}

void child_process(char **to_execute, Token *cmds, char **env)
{
	int fd_in;
	int fd_out;

	if (cmds->input_files)
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

void no_pipes(Token *cmds, char **envp)
{
	int pid;
	char **to_execute;

	to_execute = ft_split(cmds->value, ' ');
	pid = fork();
	if (pid == 0)
		child_process(to_execute, cmds, env);
	waitpid(pid, NULL, 0);
	free_a_arrays(to_execute);
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
	char *path;
	char *input;
	Token *cmds;

	while (1)
	{
		input = ft_prompt();
		cmds = parsing(input);
		after_receiving_cmds(cmds, envp);
		free(input);
		free_list(cmds); // if needed
	}
}
