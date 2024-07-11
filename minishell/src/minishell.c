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


Token *parse(char *input)
{
    Token *list = malloc(sizeof(Token));
    list->value = NULL;
    list->next = NULL;

    while(*input)
    {
        list->value = input;
        ft_printf("%c",input);
        ++input;
    }


    return list;
}
void printLinkedList(Token *head)
{
    Token *current = head;
    while (current != NULL)
    {
        // Print the value of the current node
        printf("%s ", current->value);
        
        // Move to the next node
        current = current->next;
    }
    printf("\n");
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