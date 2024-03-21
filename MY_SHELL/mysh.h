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
char *argv[MAX_CMD];
char cdform[100][BUFFSIZE]; //cd -
int cd_num = 0;
char username[BUFFSIZE];
char hostname[BUFFSIZE];
char curPath[BUFFSIZE];

// 内置的状态码
enum {
	RESULT_NORMAL,
	ERROR_FORK,
	ERROR_COMMAND,
	ERROR_WRONG_PARAMETER,
	ERROR_MISS_PARAMETER,
	ERROR_TOO_MANY_PARAMETER,
	ERROR_CD,
	ERROR_SYSTEM,
	ERROR_EXIT,

	/* 重定向的错误信息 */
	ERROR_MANY_IN,
	ERROR_MANY_OUT,
	ERROR_FILE_NOT_EXIST,
	
	/* 管道的错误信息 */
	ERROR_PIPE,
	ERROR_PIPE_MISS_PARAMETER
};
