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
#include <errno.h>
#include <pwd.h>

#define MAX_CMD 64
#define BUFFSIZE 255
#define MAX_CMD_LEN 100
#define MYSH_TOK_DELIM " \t\r\n"

char * mysh_read_line();
char ** mysh_split_line();
int mysh_execute(char **); 
int mysh_builtin_nums();
int mysh_cd(char **);
int mysh_help(char **);
int mysh_exit(char **);
int cmd_WithRedi(int, int);
int cmd_WithPipe(int , int);
int cmd_InBackground(char **, int);

char sh_path[BUFFSIZE];
char **command = NULL;
char *buf; //参数数组
char backupbuf[BUFFSIZE];  //备份参数数组
int argc;  //命令参数的数目
char cdform[1000][BUFFSIZE]; //cd -
int cd_num = 0;  //统计cd过的路径
char username[BUFFSIZE];
char hostname[BUFFSIZE];
char curPath[BUFFSIZE];
