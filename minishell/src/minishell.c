# include "../include/minishell.h"



char *ft_prompt()
{
	char *v_return;

	v_return = readline("minishell>: ");
	if (ft_strlen(v_return) == 0)
		return (free(v_return), ft_printf("\n"), ft_prompt());
	add_history(v_return);
	return (v_return);
}

Token *new_token()
{
    Token *new_node;

    new_node = malloc(sizeof(Token));
    new_node->value = NULL;
    new_node->type = TOKEN_WORD;
    new_node->input_file = NULL;
    new_node->output_file = NULL;
    new_node->append = 0;
    new_node->pipe_to_next_token = 0;
    new_node->next = NULL;
    return (new_node);

}


Token *parse(char *input)
{
    Token *list = NULL;
    char **splited = ft_split(input, ' ');
    Token **current_ptr = &list;
    for (int j = 0; splited[j] != NULL; j++) {
        *current_ptr = new_token();
        (*current_ptr)->value = ft_strdup(splited[j]);

        current_ptr = &((*current_ptr)->next);
    }
    *current_ptr = NULL;
    for (int j = 0; splited[j] != NULL; j++) {
        free(splited[j]);
    }
    free(splited);

    return list;
}

void printLinkedList(Token *head)
{
    Token *current = head;


    while (current != NULL)
    {
  
        printf("%s\n", current->value);

        current = current->next;
    }

    // printf("\n");
}

int main(int ac, char **av, char **ev)
{
    (void)ac;
    (void)av;
    (void)ev;
    char    *input;
    Token   *cmds;

    while (1)
    {
        input = ft_prompt();
        cmds = parse(input);
        printLinkedList(cmds);

    }
    ft_printf("exit\n");
    return 1;
}