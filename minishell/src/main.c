
#include "../include/minishell.h"


int main(int ac, char **av, char **envp) {
    (void)ac; (void)av;
    char *input;
    t_lexer *tokens = NULL;
    t_command *commands = NULL;
    char **my_env;

    my_env = ft_array_strdup(envp);
    while (1) {
        input = ft_prompt();
        signal(SIGINT, sig_handler);
        signal(SIGQUIT, SIG_IGN);

        if (process_input(input))
            continue;

        // Expand variables in the input
        char *expanded_input = expander(input, my_env);
        if (!expanded_input) {
            print_error("Expansion failed", NULL, 1);
            free(input);
            return 1;
        }

        tokens = (t_lexer *)tokenizer(expanded_input);
        if (!tokens) {
            print_error("Tokenizer failed", NULL, 1);
            free(input);
            continue;
        }

        commands = parser(tokens);

        // if (!check_redirections(commands)) {
        //     free_command_list(commands);
        //     free_token_list(tokens);
        //     free(input);
        //     continue;
        // }

        print_command_list(commands);
        after_receiving_cmds(commands, my_env);

        free_command_list(commands);
        free_token_list(tokens);
        free(expanded_input);
    }

    return 0;
}

