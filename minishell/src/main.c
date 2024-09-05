
#include "../include/minishell.h"



int main(int ac, char **av, char **envp) {
    (void)ac; (void)av;
    // (void)envp;
    char *input;
    Token *cmds;
    char **my_env;
    my_env = ft_array_strdup(envp);
    while (1) {
        input = ft_prompt();
        signal(SIGINT, sig_handler);
        signal(SIGQUIT, SIG_IGN);



        if (process_input(input))
            continue;
        // Expand variables in the input
            char *expanded_input = expander(input, envp);
            if (!expanded_input) {
                print_error("Expansion failed", NULL, 1);
                free(input);
                return 1;
            }

        cmds = tokenizer(expanded_input);
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
        print_token_list(cmds);
        after_receiving_cmds(cmds, my_env);

        // handle_commands(cmds);

        // free_list(cmds);
        free_token_list(cmds);
        free(expanded_input);
        // free(input);
    }

    return 0;
}
