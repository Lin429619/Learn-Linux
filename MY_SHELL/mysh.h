# include<stdio.h>
# include<stdlib.h>
# include<unistd.h>
# include<signal.h>
# include<sys/wait.h>
# include<string.h> 

# define is_delim(x) ((x) == ' ' || (x) == '\t')

char * next_cmd(char *,FILE*);
char ** splitline(char *);
void freelist(char**);
void *emalloc(size_t);
void *erealloc(void *,size_t);


