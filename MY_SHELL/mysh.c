#include "mysh.h"

char sh_path[BUFFSIZE];
char **command;
char *buf; //参数数组
char backupbuf[BUFFSIZE];  //备份参数数组
char history[MAX_CMD][BUFFSIZE];
int cmd_num = 0;
int argc;

char * builtin_cmd[] = 
{
    "cd",
    "help",
    "exit",
    "history"
};

int (*builtin_func[])(char **) = 
{
    &mysh_cd,
    &mysh_help,
    &mysh_exit
};

// 读入命令行参数，并做处理，储存
char * mysh_read_line()
{
    char * buf = NULL;
    ssize_t bufsize;
    getline(&buf, &bufsize, stdin);
    strcpy(backupbuf, buf);
    return buf;
}

//处理备份参数数组
char ** mysh_split_line(char * buf)
{
    int buffer_size = MAX_CMD, position = 0;
    char **tokens = malloc(buffer_size * sizeof(char *));
    char *token;
    token = strtok(buf, MYSH_TOK_DELIM);
    while(token != NULL)
    {
        tokens[position++] = token;
        token = strtok(NULL, MYSH_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

int main(int argc, char *argv[])
{
    int status;

    do
    {
        getcwd(sh_path, BUFFSIZE);
        char now[300] = "[mysh ";
        strcat(now, sh_path);
        strcat(now, " ]$");
        printf("%s", now);

        buf = mysh_read_line();
        command = mysh_split_line(backupbuf);
        status = mysh_execute(command);
        strcpy(history[cmd_num++], command);

        free(buf);
        free(command);
    } while (status);
    
    return 0;
}

int mysh_execute(char **command)
{
    if(command[0] == NULL) return 1;

    //先识别重定向，管道等指令

    //识别重定向输出
    for(int i = 0; i < MAX_CMD; i++){
        if(strcmp(command[i], ">") == 0){
            int aaa = cmd_OutPut(buf);
            return 1;
        }
    }

    //识别重定向输入
    for(int i = 0; i < MAX_CMD; i++){
        if(strcmp(command[i], "<") == 0){
            int aaa = cmd_InPut(buf);
            return 1;
        }
    }

    //识别追加写重定向
    for(int i = 0; i < MAX_CMD; i++){
        if(strcmp(command[i], ">>") == 0){
            int aaa = cmd_ReOutPut(buf);
            return 1;
        }
    }

    //识别管道
    for(int i = 0; i < MAX_CMD; i++){
        if(strcmp(command[i], "|") == 0){
            int aaa = cmd_Pipe(buf);
            return 1;
        }
    }

    //识别后台运行
    for(int i = 0; i < MAX_CMD; i++){
        if(strcmp(command[i], "&") == 0){
            int aaa = cmd_InBackground(buf);
            return 1;
        }
    }

    //识别内置命令
    for (int i = 0; i < mysh_builtin_nums(); i++)
        if(strcmp(command[0], builtin_cmd[i]) == 0)
            return (*builtin_func[i])(command);
    return mysh_launch(command);
}

int mysh_launch(char **command)
{
    pid_t pid, wpid;
    int status;

    pid = fork();
    if(pid == -1)
        perror("Mysh error at fork.\n");
    else if(pid == 0)
    {
        if(execvp(command[0], command) == -1)
            perror("Mysh error at execvp.\n");
        exit(EXIT_FAILURE);
    }
    else {
        do
        {
            wpid = waitpid(pid, &status, WUNTRACED);
        }while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int mysh_builtin_nums(){
    return sizeof(builtin_cmd) / sizeof(builtin_cmd[0]);
}

//cd待补充！！！！
int mysh_cd(char **command){
    if(command[1] == NULL) {
        perror("Mysh error at cd, lack of args.\n");
    }
    else{
        if(chdir(command[1]) != 0)
            perror("Mysh error at chdir.\n");
    }
    return 1;
}

int mysh_help(char **command){
    puts("This is Lin's shell.");
    puts("Here are some built in cmd:");
    for (int i = 0; i < mysh_builtin_nums(); i++)
        printf("%s\n", builtin_cmd[i]);
    return 1;
}

//待测试
int mysh_history(char **command){
    for(int i = 0; i < cmd_num; i++){
        printf("%s\n",history[i]);
    }
    return 1;
}

int mysh_exit(char **command){
    return 0;
}

int cmd_OutPut(char *buf){
    char outFile[BUFFSIZE];
    memset(outFile, 0x00, BUFFSIZE);
    int num; //统计重定向符号的数量
    for(int i = 0; i + 1 < strlen(buf); i++){
        if(buf[i] == '>' && buf[i + 1] == ' '){
            num++;
            break;
        }
    }
    if(num != 1){
        printf("输出重定向指令输入错误\n");
        return 0;
    }
    for(int i = 0; i < argc; i++){
        if(strcmp(command[i], ">") == 0){
            if(i + 1 < argc){
                strcpy(outFile, command[i + 1]);
            }else{
                printf("缺少输出文件\n");
                return 0;
            }
        }
    }

    int j;
    for(j = 0; j < strlen(buf); j++){
        if(buf[j] == '>'){
            break;
        }
    }
    buf[j - 1] = '\0';
    buf[j] = '\0';

    pid_t pid,wpid;
    int status;

    pid = fork();
    if(pid == -1)
        perror("Mysh error at OutPut_fork.\n");
    else if(pid == 0)
    {
        int fd;
        fd = open(outFile, O_WRONLY | O_CREAT | O_TRUNC, 7777);
        if(fd < 0)
            exit(1);
        dup2(fd, STDOUT_FILENO);
        execvp(command[0], command);
        if(fd != STDOUT_FILENO){
            close(fd);
        }
        printf("%s:error.\n",command[0]);
        exit(1);
    }
    else {
        do
        {
            wpid = waitpid(pid, &status, WUNTRACED);
        }while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }

}


