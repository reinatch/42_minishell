#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <errno.h>

volatile sig_atomic_t signal_received = 0;

void handle_signal(int signal) {
    signal_received = signal;
}

char *read_input() {
    char *input = readline("myshell> ");
    if (input && *input) {
        add_history(input);
    }
    return input;
}

char **split_input(char *input) {
    int bufsize = 64, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "myshell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(input, " \t\r\n\a");
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += 64;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "myshell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, " \t\r\n\a");
    }
    tokens[position] = NULL;
    return tokens;
}

char *resolve_path(char *command) {
    char *path = getenv("PATH");
    char *token;
    char full_path[1024];

    token = strtok(path, ":");
    while (token != NULL) {
        snprintf(full_path, sizeof(full_path), "%s/%s", token, command);
        if (access(full_path, X_OK) == 0) {
            return strdup(full_path);
        }
        token = strtok(NULL, ":");
    }
    return NULL;
}

int execute_command(char **args) {
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        char *path = resolve_path(args[0]);
        if (path == NULL) {
            fprintf(stderr, "myshell: command not found: %s\n", args[0]);
            exit(EXIT_FAILURE);
        }
        if (execve(path, args, environ) == -1) {
            perror("myshell");
        }
        free(path);
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("myshell");
    } else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int myshell_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "myshell: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("myshell");
        }
    }
    return 1;
}

int myshell_exit(char **args) {
    return 0;
}

char *builtin_str[] = {
    "cd",
    "exit"
};

int (*builtin_func[]) (char **) = {
    &myshell_cd,
    &myshell_exit
};

int num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

int execute_builtin(char **args) {
    for (int i = 0; i < num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }
    return execute_command(args);
}

int main() {
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, NULL);  // Handle Ctrl-C
    sigaction(SIGQUIT, &sa, NULL); // Handle Ctrl-\

    char *input;
    char **args;
    int status = 1;

    do {
        input = read_input();
        if (input == NULL) {
            break;
        }

        args = split_input(input);
        status = execute_builtin(args);

        free(input);
        free(args);
    } while (status);

    return 0;
}
