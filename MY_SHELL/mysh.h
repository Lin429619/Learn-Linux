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
#include <pwd.h>

#define MAX_CMD 64
#define BUFFSIZE 256
#define MYSH_TOK_DELIM " \t\r\n"

char * mysh_read_line();
char ** mysh_split_line(char *);
int mysh_execute(char **); 
int mysh_builtin_nums();
int mysh_cd(char **);
int mysh_help(char **);
int mysh_exit(char **);
int cmd_WithRedi(int, int);
int cmd_WithPipe(int , int);
int cmd_InBackground(char **, int);

char **command = NULL; //处理过后的命令行数组
char *buf; //命令行参数数组
char backupbuf[BUFFSIZE];  //备份参数数组
int argc;  //命令行参数的数目
char cd_former[BUFFSIZE]; //储存上一个工作路径
char username[BUFFSIZE];
char hostname[BUFFSIZE];
char curpath[BUFFSIZE];
