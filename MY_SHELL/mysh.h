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






/* # define is_delim(x) ((x) == ' ' || (x) == '\t')

char * next_cmd(char *,FILE*);
char ** splitline(char *);
void freelist(char**);
void *emalloc(size_t);
void *erealloc(void *,size_t);
void fatal(char *,char *,int);
int execute(char *argv[]); */


