
#include "../include/minishell.h"



int main(int ac, char **av, char **envp) {
    (void)ac; (void)av;
    // (void)envp;
    char *input;
    Token *cmds;
    char **my_env;
    my_env = ft_array_strdup(envp);
    while (1) {
        signal(SIGINT, sig_handler);
        signal(SIGQUIT, SIG_IGN);

        input = ft_prompt();

        if (process_input(input))
            continue;

        cmds = tokenizer(input);
        if (!cmds) {
            print_error("Tokenizer failed", NULL, 1);
            free(input);
            continue;
        }

        parser(cmds);

        if (!check_redirections(cmds)) {
            free_token_list(cmds);
            free(input);
            continue;
        }
        after_receiving_cmds(cmds, my_env);

        // handle_commands(cmds);

        // print_token_list(cmds);
        // free_list(cmds);
        // free_token_list(cmds);
        // free(input);
    }

    return 0;
}
