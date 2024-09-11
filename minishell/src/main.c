
#include "../include/minishell.h"

int main(int ac, char **av, char **envp) {
    (void)ac; (void)av;
    char *input;
    char *expanded_input;
    t_program *program = malloc(sizeof(t_program));

    program->my_env = ft_array_strdup(envp);
    program->tokens = NULL;
    program->commands = NULL;

    while (1) {
        input = ft_prompt();
        signal(SIGINT, sig_handler);
        signal(SIGQUIT, SIG_IGN);
        if (process_input(input)) {
            free(input);
            continue;
        }
        expanded_input = expander(input, program);
        if (!expanded_input) {
            print_error("Expansion failed", NULL, 1);
            free(input);
            continue;
        }
        program->tokens = tokenizer(expanded_input);
        if (!program->tokens) {
            print_error("Tokenizer failed", NULL, 1);
            free(input);
            free(expanded_input);
            continue;
        }
        program = parser(program);

        if (!program->commands) {
            print_error("Parser failed", NULL, 1);
            free_token_list(program->tokens);
            free(input);
            free(expanded_input);
            continue;
        }

        // Check redirections (uncomment if needed)
        // if (!check_redirections(program->commands)) {
        //     free_command_list(program->commands);
        //     free_token_list(program->tokens);
        //     free(input);
        //     free(expanded_input);
        //     continue;
        // }

        // Print the command list for debugging/verification
        print_command_list(program->commands);

        after_receiving_cmds(program->commands, program->my_env);

        free_command_list(program->commands);
        free_token_list(program->tokens);
        free(expanded_input);
        free(input);
    }

    free(program->my_env);
    free(program);
    
    return 0;
}
