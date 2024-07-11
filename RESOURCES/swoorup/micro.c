#ifndef ASTREE_H
#define ASTREE_H

typedef enum {
    NODE_PIPE 			= (1 << 0),
    NODE_BCKGRND 		= (1 << 1),
    NODE_SEQ 			= (1 << 2),
    NODE_REDIRECT_IN 	= (1 << 3),
    NODE_REDIRECT_OUT 	= (1 << 4),
    NODE_CMDPATH		= (1 << 5),
    NODE_ARGUMENT		= (1 << 6),

    NODE_DATA 			= (1 << 7),
} NodeType;

typedef struct ASTreeNode
{
    int type;
    char* szData;
    struct ASTreeNode* left;
    struct ASTreeNode* right;

} ASTreeNode;

#define NODETYPE(a) (a & (~NODE_DATA))	// get the type of the nodes

void ASTreeAttachBinaryBranch (ASTreeNode * root , ASTreeNode * leftNode , ASTreeNode * rightNode);
void ASTreeNodeSetType (ASTreeNode * node , NodeType nodetype );
void ASTreeNodeSetData (ASTreeNode * node , char * data );
void ASTreeNodeDelete (ASTreeNode * node );

#endif
#ifndef COMMAND_H
#define COMMAND_H

#include <unistd.h>
#include <stdbool.h>
#include "astree.h"

struct CommandInternal
{
	int argc;
	char **argv;
	bool stdin_pipe;
	bool stdout_pipe;
	int pipe_read;
	int pipe_write;
	char* redirect_in;
	char* redirect_out;
	bool asynchrnous;
};

typedef struct CommandInternal CommandInternal;

void set_prompt(char* str);
char* getprompt();
void ignore_signal_for_shell();
void execute_cd(CommandInternal* cmdinternal);
void execute_prompt(CommandInternal* cmdinternal);
void execute_pwd(CommandInternal* cmdinternal);
void execute_command_internal(CommandInternal* cmdinternal);
int init_command_internal(ASTreeNode* simplecmdNode, 
						  CommandInternal* cmdinternal, 
						  bool async,
						  bool stdin_pipe,
						  bool stdout_pipe,
						  int pipe_read,
						  int pipe_write,
						  char* redirect_in,
						  char* redirect_out
);
void destroy_command_internal(CommandInternal* cmdinternal);

#endif

#ifndef EXECUTE_H
#define EXECUTE_H

#include "astree.h"
#include <stdbool.h>

void execute_syntax_tree(ASTreeNode* tree);

#endif

#ifndef LEXER_H
#define LEXER_H

enum TokenType{
	CHAR_GENERAL = -1,
	CHAR_PIPE = '|',
	CHAR_AMPERSAND = '&',
	CHAR_QOUTE = '\'',
	CHAR_DQUOTE = '\"',
	CHAR_SEMICOLON = ';',
	CHAR_WHITESPACE = ' ',
	CHAR_ESCAPESEQUENCE = '\\',
	CHAR_TAB = '\t',
	CHAR_NEWLINE = '\n',
	CHAR_GREATER = '>',
	CHAR_LESSER = '<',
	CHAR_NULL = 0,
	
	TOKEN	= -1,
};

enum {
	STATE_IN_DQUOTE,
	STATE_IN_QUOTE,
	
	STATE_IN_ESCAPESEQ,
	STATE_GENERAL,
};

typedef struct tok tok_t;
typedef struct lexer lexer_t;

struct tok {
	char* data;
	int type;
	tok_t* next;
};

struct lexer
{
	tok_t* llisttok;
	int ntoks;
};

int lexer_build(char* input, int size, lexer_t* lexerbuf);
void lexer_destroy(lexer_t* lexerbuf);

#endif

#ifndef PARSER_H
#define PARSER_H

#include "astree.h"
#include "lexer.h"

int parse(lexer_t* lexbuf, ASTreeNode** syntax_tree);

#endif
#include "astree.h"
#include <assert.h>
#include <stdlib.h>

void ASTreeAttachBinaryBranch(ASTreeNode* root, ASTreeNode* leftNode, ASTreeNode* rightNode)
{
    assert(root != NULL);
    root->left = leftNode;
    root->right = rightNode;
}

void ASTreeNodeSetType(ASTreeNode* node, NodeType nodetype)
{
    assert(node != NULL);
    node->type = nodetype;
}

void ASTreeNodeSetData(ASTreeNode* node, char* data)
{
    assert(node != NULL);
    if(data != NULL) {
        node->szData = data;
        node->type |= NODE_DATA;
    }
}

void ASTreeNodeDelete(ASTreeNode* node)
{
    if (node == NULL)
        return;

    if (node->type & NODE_DATA)
        free(node->szData);


    ASTreeNodeDelete(node->left);
    ASTreeNodeDelete(node->right);
    free(node);
}
#include "command.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/wait.h>
#include <pwd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>

char* prompt = NULL;
bool signalset = false;
void   (*SIGINT_handler)(int);

void set_prompt(char* str)
{
    if (prompt != NULL)
        free(prompt);

    prompt = malloc(strlen(str) + 1);
    strcpy(prompt, str);
}

char* getprompt()
{
	return prompt;
}

void ignore_signal_for_shell()
{
	signalset = true;
	
	// ignore "Ctrl-C"
    SIGINT_handler = signal(SIGINT, SIG_IGN);
	// ignore "Ctrl-Z"
    signal(SIGTSTP, SIG_IGN);
	// ignore "Ctrl-\"
    signal(SIGQUIT, SIG_IGN);
}

// restore Ctrl-C signal in the child process
void restore_sigint_in_child()
{
	if (signalset)
		signal(SIGINT, SIGINT_handler);
}

// built-in command cd
void execute_cd(CommandInternal* cmdinternal)
{
    if (cmdinternal->argc == 1) {
		struct passwd *pw = getpwuid(getuid());
		const char *homedir = pw->pw_dir;
		chdir(homedir);
	}
    else if (cmdinternal->argc > 2)
        printf("cd: Too many arguments\n");
    else {
        if (chdir(cmdinternal->argv[1]) != 0)
            perror(cmdinternal->argv[1]);
    }
}

// built-in command prompt
void execute_prompt(CommandInternal* cmdinternal)
{
    if (cmdinternal->argc == 1)
        printf("prompt: Please specify the prompt string\n");
    else if (cmdinternal->argc > 2)
        printf("prompt: Too many arguments\n");
    else {
        set_prompt(cmdinternal->argv[1]);
    }
}

// built-in command pwd
void execute_pwd(CommandInternal* cmdinternal)
{
    pid_t pid;
    if((pid = fork()) == 0 ) {
        if (cmdinternal->redirect_out) {
            int fd = open(cmdinternal->redirect_out, O_WRONLY | O_CREAT | O_TRUNC,
                          S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if (fd == -1) {
                perror(cmdinternal->redirect_out);
                exit(1);
            }

            dup2(fd, STDOUT_FILENO);
        }

        if (cmdinternal->stdout_pipe)
			dup2(cmdinternal->pipe_write, STDOUT_FILENO);

        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL)
            fprintf(stdout, "%s\n", cwd);
        else
            perror("getcwd() error");

        exit(0);
    }
    else if (pid < 0) {
        perror("fork");
        return;
    }
    else
		while (waitpid(pid, NULL, 0) <= 0);

    return;
}

void zombie_process_handler(int signum)
{
    int more = 1;        // more zombies to claim
    pid_t pid;           // pid of the zombie
    int status;          // termination status of the zombie

    while (more) {
        pid = waitpid(-1, &status, WNOHANG);
        if (pid > 0)
            printf("\n%d terminated\n", pid);
        if (pid<=0)
            more = 0;
    }
}

void execute_command_internal(CommandInternal* cmdinternal)
{

    if (cmdinternal->argc < 0)
        return;

    // check for built-in commands
    if (strcmp(cmdinternal->argv[0], "cd") == 0) {
        execute_cd(cmdinternal);
        return;
    }
    else if (strcmp(cmdinternal->argv[0], "prompt") == 0) {
		execute_prompt(cmdinternal);
        return;
	}
    else if (strcmp(cmdinternal->argv[0], "pwd") == 0)
        return execute_pwd(cmdinternal);
    else if(strcmp(cmdinternal->argv[0], "exit") == 0) {
		exit(0);
		return;
	}

    pid_t pid;
    if((pid = fork()) == 0 ) {
		// restore the signals in the child process
		restore_sigint_in_child();
		
		// store the stdout file desc
        int stdoutfd = dup(STDOUT_FILENO);

		// for bckgrnd jobs redirect stdin from /dev/null
        if (cmdinternal->asynchrnous) {
            int fd = open("/dev/null",O_RDWR);
            if (fd == -1) {
                perror("/dev/null");
                exit(1);
            }
            dup2(fd, STDIN_FILENO);
        }

        // redirect stdin from file if specified
        if (cmdinternal->redirect_in) {
            int fd = open(cmdinternal->redirect_in, O_RDONLY);
            if (fd == -1) {
                perror(cmdinternal->redirect_in);
                exit(1);
            }

            dup2(fd, STDIN_FILENO);
        }

        // redirect stdout to file if specified
        else if (cmdinternal->redirect_out) {
            int fd = open(cmdinternal->redirect_out, O_WRONLY | O_CREAT | O_TRUNC,
                          S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if (fd == -1) {
                perror(cmdinternal->redirect_out);
                exit(1);
            }

            dup2(fd, STDOUT_FILENO);
        }

        // read stdin from pipe if present
        if (cmdinternal->stdin_pipe)
            dup2(cmdinternal->pipe_read, STDIN_FILENO);

		// write stdout to pipe if present
        if (cmdinternal->stdout_pipe)
            dup2(cmdinternal->pipe_write, STDOUT_FILENO);

        if (execvp(cmdinternal->argv[0], cmdinternal->argv) == -1) {
			// restore the stdout for displaying error message
            dup2(stdoutfd, STDOUT_FILENO);
			
            printf("Command not found: \'%s\'\n", cmdinternal->argv[0]);
			exit(1);
        }

        
    }
    else if (pid < 0) {
        perror("fork");
        return;
    }

    if (!cmdinternal->asynchrnous)
    {
        // wait till the process has not finished
        while (waitpid(pid, NULL, 0) <= 0);
    }
    else
    {
		// set the sigchild handler for the spawned process
        printf("%d started\n", pid);
        struct sigaction act;
        act.sa_flags = 0;
        act.sa_handler = zombie_process_handler;
        sigfillset( & (act.sa_mask) ); // to block all

        if (sigaction(SIGCHLD, &act, NULL) != 0)
            perror("sigaction");
    }

    return;
}

int init_command_internal(ASTreeNode* simplecmdNode,
                          CommandInternal* cmdinternal,
                          bool async,
                          bool stdin_pipe,
                          bool stdout_pipe,
                          int pipe_read,
                          int pipe_write,
                          char* redirect_in,
                          char* redirect_out)
{
    if (simplecmdNode == NULL || !(NODETYPE(simplecmdNode->type) == NODE_CMDPATH))
    {
        cmdinternal->argc = 0;
        return -1;
    }

    ASTreeNode* argNode = simplecmdNode;

    int i = 0;
    while (argNode != NULL && (NODETYPE(argNode->type) == NODE_ARGUMENT || NODETYPE(argNode->type) == NODE_CMDPATH)) {
        argNode = argNode->right;
        i++;
    }

    cmdinternal->argv = (char**)malloc(sizeof(char*) * (i + 1));
    argNode = simplecmdNode;
    i = 0;
    while (argNode != NULL && (NODETYPE(argNode->type) == NODE_ARGUMENT || NODETYPE(argNode->type) == NODE_CMDPATH)) {
        cmdinternal->argv[i] = (char*)malloc(strlen(argNode->szData) + 1);
        strcpy(cmdinternal->argv[i], argNode->szData);

        argNode = argNode->right;
        i++;
    }

    cmdinternal->argv[i] = NULL;
    cmdinternal->argc = i;

    cmdinternal->asynchrnous = async;
    cmdinternal->stdin_pipe = stdin_pipe;
    cmdinternal->stdout_pipe = stdout_pipe;
    cmdinternal->pipe_read = pipe_read;
    cmdinternal->pipe_write = pipe_write;
    cmdinternal->redirect_in = redirect_in;
    cmdinternal->redirect_out = redirect_out;

    return 0;
}

void destroy_command_internal(CommandInternal* cmdinternal)
{
    int i;
    for (i = 0; i < cmdinternal->argc; i++)
        free(cmdinternal->argv[i]);

    free(cmdinternal->argv);
    cmdinternal->argc = 0;
}
#include "command.h"
#include <unistd.h>
#include <stdbool.h>

void execute_simple_command(ASTreeNode* simple_cmd_node,
                             bool async,
                             bool stdin_pipe,
                             bool stdout_pipe,
                             int pipe_read,
                             int pipe_write,
                             char* redirect_in,
                             char* redirect_out
                            )
{
    CommandInternal cmdinternal;
    init_command_internal(simple_cmd_node, &cmdinternal, async, stdin_pipe, stdout_pipe,
                          pipe_read, pipe_write, redirect_in, redirect_out
                         );
	execute_command_internal(&cmdinternal);
	destroy_command_internal(&cmdinternal);
}

void execute_command(ASTreeNode* cmdNode,
                      bool async,
                      bool stdin_pipe,
                      bool stdout_pipe,
                      int pipe_read,
                      int pipe_write)
{
    if (cmdNode == NULL)
        return;

    switch (NODETYPE(cmdNode->type))
    {
    case NODE_REDIRECT_IN:		// right side contains simple cmd node
        execute_simple_command(cmdNode->right,
                               async,
                               stdin_pipe,
                               stdout_pipe,
                               pipe_read,
                               pipe_write,
                               cmdNode->szData, NULL
                              );
        break;
    case NODE_REDIRECT_OUT:		// right side contains simple cmd node
        execute_simple_command(cmdNode->right,
                               async,
                               stdin_pipe,
                               stdout_pipe,
                               pipe_read,
                               pipe_write,
                               NULL, cmdNode->szData
                              );
        break;
    case NODE_CMDPATH:
        execute_simple_command(cmdNode,
                               async,
                               stdin_pipe,
                               stdout_pipe,
                               pipe_read,
                               pipe_write,
                               NULL, NULL
                              );
        break;
    }
}

void execute_pipeline(ASTreeNode* t, bool async)
{
    int file_desc[2];

    pipe(file_desc);
    int pipewrite = file_desc[1];
    int piperead = file_desc[0];

	// read input from stdin for the first job
    execute_command(t->left, async, false, true, 0, pipewrite);
    ASTreeNode* jobNode = t->right;

    while (jobNode != NULL && NODETYPE(jobNode->type) == NODE_PIPE)
    {
        close(pipewrite); // close the write end
        pipe(file_desc);
        pipewrite = file_desc[1];

        execute_command(jobNode->left, async, true, true, piperead, pipewrite);
        close(piperead);
        piperead = file_desc[0];

        jobNode = jobNode->right;
    }

    piperead = file_desc[0];
    close(pipewrite);
	
	// write output to stdout for the last job
    execute_command(jobNode, async, true, false, piperead, 0);	// only wait for the last command if necessary
    close(piperead);
}

void execute_job(ASTreeNode* jobNode, bool async)
{
    if (jobNode == NULL)
        return;

    switch (NODETYPE(jobNode->type))
    {
    case NODE_PIPE:
        execute_pipeline(jobNode, async);
        break;
    case NODE_CMDPATH:
    default:
        execute_command(jobNode, async, false, false, 0, 0);
        break;
    }
}

void execute_cmdline(ASTreeNode* cmdline)
{
    if (cmdline == NULL)
        return;

    switch(NODETYPE(cmdline->type))
    {
    case NODE_SEQ:
        execute_job(cmdline->left, false);
        execute_cmdline(cmdline->right);
        break;

    case NODE_BCKGRND:
        execute_job(cmdline->left, true);  // job to be background
        execute_cmdline(cmdline->right);
        break;
    default:
        execute_job(cmdline, false);
    }
}

void execute_syntax_tree(ASTreeNode* tree)
{
	// interpret the syntax tree
    execute_cmdline(tree);
}
#include <glob.h>
#include <string.h>
#include <stdlib.h>
#include "lexer.h"

int getchartype(char c)
{
	switch(c)
	{
		case '\'':
			return CHAR_QOUTE;
			break;
		case '\"':
			return CHAR_DQUOTE;
			break;
		case '|':
			return CHAR_PIPE;
			break;
		case '&':
			return CHAR_AMPERSAND;
			break;
		case ' ':
			return CHAR_WHITESPACE;
			break;
		case ';':
			return CHAR_SEMICOLON;
			break;
		case '\\':
			return CHAR_ESCAPESEQUENCE;
			break;
		case '\t':
			return CHAR_TAB;
			break;
		case '\n':
			return CHAR_NEWLINE;
			break;
		case '>':
			return CHAR_GREATER;
			break;
		case '<':
			return CHAR_LESSER;
			break;
		case 0:
			return CHAR_NULL;
			break;
		default:
			return CHAR_GENERAL;
	};
	
	return CHAR_GENERAL;
}

void strip_quotes(char* src, char* dest)
{
	int n = strlen(src);
	if (n <= 1) {
		strcpy(dest, src);
		return;
	}
	
	int i;
	
	char lastquote = 0;
	int j = 0;
	
	for (i=0; i < n; i++)
	{
		char c = src[i];
		if ((c == '\'' || c == '\"') && lastquote == 0)
			lastquote = c;
		else if (c == lastquote)
			lastquote = 0;
		else
			dest[j++] = c;
	}
	
	dest[j] = 0;
}

void tok_init(tok_t* tok, int datasize)
{
	tok->data = malloc(datasize + 1); // 1 for null terminator
	tok->data[0] = 0;
	
	tok->type = CHAR_NULL;
	tok->next = NULL;
}

void tok_destroy(tok_t* tok) {
	if (tok != NULL) {
		free(tok->data);
		tok_destroy(tok->next);
		free(tok);
	}
}

int lexer_build(char* input, int size, lexer_t* lexerbuf)
{
	if (lexerbuf == NULL)
		return -1;
	
	if (size == 0) {
		lexerbuf->ntoks = 0;
		return 0;
	}
	
	lexerbuf->llisttok = malloc(sizeof(tok_t));
	
	// allocate the first token
	tok_t* token = lexerbuf->llisttok;
	tok_init(token, size);
	
	int i = 0;
	int j = 0, ntemptok = 0;
	
	char c;
	int state = STATE_GENERAL;
	
	do
	{
		c = input[i];		
		int chtype = getchartype(c);
		
		if (state == STATE_GENERAL)
		{
			switch (chtype) 
			{
				case CHAR_QOUTE:
					state = STATE_IN_QUOTE;
					token->data[j++] = CHAR_QOUTE;
					token->type = TOKEN;
					break;
					
				case CHAR_DQUOTE:
					state = STATE_IN_DQUOTE;
					token->data[j++] = CHAR_DQUOTE;
					token->type = TOKEN;
					break;
					
				case CHAR_ESCAPESEQUENCE:
					token->data[j++] = input[++i];
					token->type = TOKEN;
					break;
					
				case CHAR_GENERAL:
					token->data[j++] = c;
					token->type = TOKEN;
					break;
					
				case CHAR_WHITESPACE:
					if (j > 0) {
						token->data[j] = 0;
						token->next = malloc(sizeof(tok_t));
						token = token->next;
						tok_init(token, size - i);
						j = 0;
					}
					break;
					
				case CHAR_SEMICOLON:
				case CHAR_GREATER:
				case CHAR_LESSER:
				case CHAR_AMPERSAND:
				case CHAR_PIPE:
					
					// end the token that was being read before
					if (j > 0) {
						token->data[j] = 0;
						token->next = malloc(sizeof(tok_t));
						token = token->next;
						tok_init(token, size - i);
						j = 0;
					}
					
					// next token
					token->data[0] = chtype;
					token->data[1] = 0;
					token->type = chtype;
					
					token->next = malloc(sizeof(tok_t));
					token = token->next;
					tok_init(token, size - i);
					break;
			}
		}
		else if (state == STATE_IN_DQUOTE) {
			token->data[j++] = c;
			if (chtype == CHAR_DQUOTE)
				state = STATE_GENERAL;
			
		}
		else if (state == STATE_IN_QUOTE) {
			token->data[j++] = c;
			if (chtype == CHAR_QOUTE)
				state = STATE_GENERAL;
		}
		
		if (chtype == CHAR_NULL) {
			if (j > 0) {
				token->data[j] = 0;
				ntemptok++;
				j = 0;
			}
		}
		
		i++;
	} while (c != '\0');
	
	token = lexerbuf->llisttok;
	int k = 0;
	while (token != NULL) 
	{
		if (token->type == TOKEN)
		{
			glob_t globbuf;
			glob(token->data, GLOB_TILDE, NULL, &globbuf);
			
			if (globbuf.gl_pathc > 0)
			{
				k += globbuf.gl_pathc;
				// save the next token so we can attach it later
				tok_t* saved = token->next;
				
				// replace the current token with the first one
				free(token->data);
				token->data = malloc(strlen(globbuf.gl_pathv[0]) + 1);
				strcpy(token->data, globbuf.gl_pathv[0]);
								
				int i;
				for (i = 1; i < globbuf.gl_pathc; i++)
				{
					token->next = malloc(sizeof(tok_t));
					tok_init(token->next, strlen(globbuf.gl_pathv[i]));
					token = token->next;
					token->type = TOKEN;
					strcpy(token->data, globbuf.gl_pathv[i]);
				}
				
				token->next = saved;
			}
			else {
				// token from the user might be inside quotation to escape special characters
				// hence strip the quotation symbol
				char* stripped = malloc(strlen(token->data) + 1);
				strip_quotes(token->data, stripped);
				free(token->data);
				token->data = stripped;
				k++;
			}
		}
		
		token = token->next;
	}
	
	lexerbuf->ntoks = k;
	return k;
}

void lexer_destroy(lexer_t* lexerbuf)
{
	if (lexerbuf == NULL)
		return;
	
	tok_destroy(lexerbuf->llisttok);
}
#include "parser.h"
#include "lexer.h"
#include "astree.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*** Shell Grammer as given in the assignment question 1 ***/

/**
 *
	<command line>	::=  	<job>
						|	<job> '&'
						| 	<job> '&' <command line>
						|	<job> ';'
						|	<job> ';' <command line>

	<job>			::=		<command>
						|	< job > '|' < command >

	<command>		::=		<simple command>
						|	<simple command> '<' <filename>
						|	<simple command> '>' <filename>

	<simple command>::=		<pathname>
						|	<simple command>  <token>
 *
 *
 *
**/

/*** Shell Grammer for recursive descent parser ***/
/*** Removed left recursion and left factoring ***/

/**
 *
	<command line>	::= 	<job> ';' <command line>
						|	<job> ';'
						| 	<job> '&' <command line>
						|	<job> '&'
							<job>

	<job>			::=		<command> '|' <job>
						|	<command>

	<command>		::=		<simple command> '<' <filename> // this grammer is a bit incorrect, see grammer.llf
						|	<simple command> '>' <filename>
						|	<simple command>

	<simple command>::=		<pathname> <token list>

	<token list>	::=		<token> <token list>
						|	(EMPTY)

 *
 *
 *
**/

ASTreeNode* CMDLINE();		//	test all command line production orderwise
ASTreeNode* CMDLINE1();		//	<job> ';' <command line>
ASTreeNode* CMDLINE2();		//	<job> ';'
ASTreeNode* CMDLINE3();		//	<job> '&' <command line>
ASTreeNode* CMDLINE4();		//	<job> '&'
ASTreeNode* CMDLINE5();		//	<job>

ASTreeNode* JOB();			// test all job production in order
ASTreeNode* JOB1();			// <command> '|' <job>
ASTreeNode* JOB2();			// <command>

ASTreeNode* CMD();			// test all command production orderwise
ASTreeNode* CMD1();			//	<simple command> '<' <filename>
ASTreeNode* CMD2();			//	<simple command> '>' <filename>
ASTreeNode* CMD3();			//	<simple command>

ASTreeNode* SIMPLECMD();	// test simple cmd production
ASTreeNode* SIMPLECMD1();	// <pathname> <token list>

ASTreeNode* TOKENLIST();	// test tokenlist production
ASTreeNode* TOKENLIST1();	//	<token> <token list>
ASTreeNode* TOKENLIST2();	//	EMPTY

// curtok token pointer
tok_t* curtok = NULL;

bool term(int toketype, char** bufferptr)
{
	if (curtok == NULL)
		return false;
	
    if (curtok->type == toketype)
    {
		if (bufferptr != NULL) {
			*bufferptr = malloc(strlen(curtok->data) + 1);
			strcpy(*bufferptr, curtok->data);
		}
		curtok = curtok->next;
        return true;
    }

    curtok = curtok->next;
    return false;
}

ASTreeNode* CMDLINE()
{
    tok_t* save = curtok;

    ASTreeNode* node;

    if ((curtok = save, node = CMDLINE1()) != NULL)
        return node;

    if ((curtok = save, node = CMDLINE2()) != NULL)
        return node;

    if ((curtok = save, node = CMDLINE3()) != NULL)
        return node;

    if ((curtok = save, node = CMDLINE4()) != NULL)
        return node;

    if ((curtok = save, node = CMDLINE5()) != NULL)
        return node;

    return NULL;
}

ASTreeNode* CMDLINE1()
{
    ASTreeNode* jobNode;
    ASTreeNode* cmdlineNode;
    ASTreeNode* result;

    if ((jobNode = JOB()) == NULL)
        return NULL;

    if (!term(CHAR_SEMICOLON, NULL)) {
        ASTreeNodeDelete(jobNode);
        return NULL;
    }

    if ((cmdlineNode = CMDLINE()) == NULL) {
        ASTreeNodeDelete(jobNode);
        return NULL;
    }

    result = malloc(sizeof(*result));
    ASTreeNodeSetType(result, NODE_SEQ);
    ASTreeAttachBinaryBranch(result, jobNode, cmdlineNode);

    return result;
}

ASTreeNode* CMDLINE2()
{
    ASTreeNode* jobNode;
    ASTreeNode* result;

    if ((jobNode = JOB()) == NULL)
        return NULL;

	if (!term(CHAR_SEMICOLON, NULL)) {
        ASTreeNodeDelete(jobNode);
        return NULL;
    }

    result = malloc(sizeof(*result));
    ASTreeNodeSetType(result, NODE_SEQ);
    ASTreeAttachBinaryBranch(result, jobNode, NULL);

    return result;
}

ASTreeNode* CMDLINE3()
{
    ASTreeNode* jobNode;
    ASTreeNode* cmdlineNode;
    ASTreeNode* result;

    if ((jobNode = JOB()) == NULL)
        return NULL;

    if (!term(CHAR_AMPERSAND, NULL)) {
        ASTreeNodeDelete(jobNode);
        return NULL;
    }

    if ((cmdlineNode = CMDLINE()) == NULL) {
        ASTreeNodeDelete(jobNode);
        return NULL;
    }

    result = malloc(sizeof(*result));
    ASTreeNodeSetType(result, NODE_BCKGRND);
    ASTreeAttachBinaryBranch(result, jobNode, cmdlineNode);

    return result;
}

ASTreeNode* CMDLINE4()
{
    ASTreeNode* jobNode;
    ASTreeNode* result;

    if ((jobNode = JOB()) == NULL)
        return NULL;

	if (!term(CHAR_AMPERSAND, NULL)) {
        ASTreeNodeDelete(jobNode);
        return NULL;
    }

    result = malloc(sizeof(*result));
    ASTreeNodeSetType(result, NODE_BCKGRND);
    ASTreeAttachBinaryBranch(result, jobNode, NULL);

    return result;
}

ASTreeNode* CMDLINE5()
{
    return JOB();
}

ASTreeNode* JOB()
{
    tok_t* save = curtok;

    ASTreeNode* node;

    if ((curtok = save, node = JOB1()) != NULL)
        return node;

    if ((curtok = save, node = JOB2()) != NULL)
        return node;

    return NULL;
}

ASTreeNode* JOB1()
{
    ASTreeNode* cmdNode;
    ASTreeNode* jobNode;
    ASTreeNode* result;

    if ((cmdNode = CMD()) == NULL)
        return NULL;

    if (!term(CHAR_PIPE, NULL)) {
        ASTreeNodeDelete(cmdNode);
        return NULL;
    }

    if ((jobNode = JOB()) == NULL) {
        ASTreeNodeDelete(cmdNode);
        return NULL;
    }

    result = malloc(sizeof(*result));
    ASTreeNodeSetType(result, NODE_PIPE);
    ASTreeAttachBinaryBranch(result, cmdNode, jobNode);

    return result;
}

ASTreeNode* JOB2()
{
    return CMD();
}

ASTreeNode* CMD()
{
    tok_t* save = curtok;

    ASTreeNode* node;

    if ((curtok = save, node = CMD1()) != NULL)
        return node;

    if ((curtok = save, node = CMD2()) != NULL)
        return node;

    if ((curtok = save, node = CMD3()) != NULL)
        return node;

    return NULL;
}

ASTreeNode* CMD1()
{
    ASTreeNode* simplecmdNode;
    ASTreeNode* result;

    if ((simplecmdNode = SIMPLECMD()) == NULL)
        return NULL;

    if (!term(CHAR_LESSER, NULL)) {
		ASTreeNodeDelete(simplecmdNode);
		return NULL;
	}
	
	char* filename;
	if (!term(TOKEN, &filename)) {
		free(filename);
        ASTreeNodeDelete(simplecmdNode);
        return NULL;
    }

    result = malloc(sizeof(*result));
    ASTreeNodeSetType(result, NODE_REDIRECT_IN);
    ASTreeNodeSetData(result, filename);
    ASTreeAttachBinaryBranch(result, NULL, simplecmdNode);

    return result;
}

ASTreeNode* CMD2()
{
    ASTreeNode* simplecmdNode;
    ASTreeNode* result;

    if ((simplecmdNode = SIMPLECMD()) == NULL)
        return NULL;

	if (!term(CHAR_GREATER, NULL)) {
		ASTreeNodeDelete(simplecmdNode);
		return NULL;
	}
	
	char* filename;
	if (!term(TOKEN, &filename)) {
		free(filename);
		ASTreeNodeDelete(simplecmdNode);
		return NULL;
	}

    result = malloc(sizeof(*result));
    ASTreeNodeSetType(result, NODE_REDIRECT_OUT);
    ASTreeNodeSetData(result, filename);
	ASTreeAttachBinaryBranch(result, NULL, simplecmdNode);

    return result;
}

ASTreeNode* CMD3()
{
    return SIMPLECMD();
}

ASTreeNode* SIMPLECMD()
{
    tok_t* save = curtok;
    return SIMPLECMD1();
}

ASTreeNode* SIMPLECMD1()
{
    ASTreeNode* tokenListNode;
    ASTreeNode* result;

    char* pathname;
    if (!term(TOKEN, &pathname))
        return NULL;

    tokenListNode = TOKENLIST();
    // we don't check whether tokenlistNode is NULL since its a valid grammer

    result = malloc(sizeof(*result));
    ASTreeNodeSetType(result, NODE_CMDPATH);
    ASTreeNodeSetData(result, pathname);
	ASTreeAttachBinaryBranch(result, NULL, tokenListNode);

    return result;
}

ASTreeNode* TOKENLIST()
{
    tok_t* save = curtok;

    ASTreeNode* node;

    if ((curtok = save, node = TOKENLIST1()) != NULL)
        return node;

    if ((curtok = save, node = TOKENLIST2()) != NULL)
        return node;

    return NULL;
}

ASTreeNode* TOKENLIST1()
{
    ASTreeNode* tokenListNode;
    ASTreeNode* result;

    char* arg;
    if (!term(TOKEN, &arg))
        return NULL;

    tokenListNode = TOKENLIST();
    // we don't check whether tokenlistNode is NULL since its a valid grammer

    result = malloc(sizeof(*result));
    ASTreeNodeSetType(result, NODE_ARGUMENT);
    ASTreeNodeSetData(result, arg);
	ASTreeAttachBinaryBranch(result, NULL, tokenListNode);

    return result;
}

ASTreeNode* TOKENLIST2()
{
    return NULL;
}

int parse(lexer_t* lexbuf, ASTreeNode** syntax_tree)
{
	if (lexbuf->ntoks == 0)
		return -1;
	
	curtok = lexbuf->llisttok;
    *syntax_tree = CMDLINE();
	
    if (curtok != NULL && curtok->type != 0)
    {
        printf("Syntax Error near: %s\n", curtok->data);
        return -1;
    }
	
	return 0;
}
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "execute.h"
#include "command.h"

int main()
{
	// ignore Ctrl-\ Ctrl-C Ctrl-Z signals
	ignore_signal_for_shell();

	// set the prompt
	set_prompt("swoorup % ");

	while (1)
	{
		char* linebuffer;
		size_t len;

		lexer_t lexerbuf;
		ASTreeNode* exectree;

		// keep getline in a loop in case interruption occurs
		int again = 1;
		while (again) {
			again = 0;
			printf("%s", getprompt());
			linebuffer = NULL;
			len = 0;
			ssize_t nread = getline(&linebuffer, &len, stdin);
			if (nread <= 0 && errno == EINTR) {
				again = 1;        	// signal interruption, read again
				clearerr(stdin);	// clear the error
			}
		}

		// user pressed ctrl-D
		if (feof(stdin)) {
			exit(0);
			return 0;
		}

		// lexically analyze and build a list of tokens
		lexer_build(linebuffer, len, &lexerbuf);
		free(linebuffer);

		// parse the tokens into an abstract syntax tree
		if (!lexerbuf.ntoks || parse(&lexerbuf, &exectree) != 0) {
			continue;
		}

		execute_syntax_tree(exectree);

		// free the structures
		ASTreeNodeDelete(exectree);
		lexer_destroy(&lexerbuf);
	}

	return 0;
}

