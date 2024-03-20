#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h> 
#include <fcntl.h>
#include <ctype.h>
#include <dirent.h>

#define MAX_CMD 64
#define BUFFSIZE 255
#define MAX_CMD_LEN 100
#define MYSH_TOK_DELIM " \t\r\n"

char * mysh_read_line();
char ** mysh_split_line();
void parse(char *);
int mysh_execute(char **); 
int mysh_launch(char **);
int mysh_builtin_nums();
int mysh_cd(char **);
int mysh_help(char **);
int mysh_exit(char **);
int mysh_history(char **);
int cmd_OutPut(char *);
int cmd_InPut(char *);
int cmd_ReOutPut(char *);
int cmd_Pipe(char *);
int cmd_InBackground(char *);

char sh_path[BUFFSIZE];
char **command = NULL;
char *buf; //参数数组
char backupbuf[BUFFSIZE];  //备份参数数组
char history[MAX_CMD][BUFFSIZE];
int cmd_num = 0;
int argc;
char *argv[BUFFSIZE];
char cdform[100][BUFFSIZE]; //cd -
int cd_num = 0;