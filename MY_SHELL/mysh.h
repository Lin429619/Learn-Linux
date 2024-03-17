#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h> 
#include <fcntl.h>

int mysh_cd(char **args);
int mysh_help(char **args);
int mysh_exit(char **args);
char ** mysh_split_line();
int mysh_execute(char **args);
