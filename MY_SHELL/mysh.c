#include "mysh.h"

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
    &mysh_exit,
    &mysh_history
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
char ** mysh_split_line(char *buf)
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
    argc = position;
    tokens[position] = NULL;
    return tokens;
}

void parse(char *buf){
    int pos = 0;
    char *token;
    //初始化数组argv
    for(int i = 0; i < MAX_CMD; i++){
        argv[i] = NULL;
    }

    token = strtok(buf, MYSH_TOK_DELIM);
    while(token != NULL)
    {
        argv[pos++] = token;
        token = strtok(NULL, MYSH_TOK_DELIM);
    }
    argv[pos] = NULL;

}

int main(int argc, char *argv[])
{
    strcpy(cdform[0], "/home/lin/");
    signal(SIGINT, SIG_IGN);  //屏蔽ctrl+c

    int status;

    do
    {
        getcwd(sh_path, BUFFSIZE);
        char now[300] = "[mysh ";
        strcat(now, sh_path);
        strcat(now, " ]$");
        printf("%s", now);

        buf = mysh_read_line();

        strcpy(history[cmd_num++], buf);

        command = mysh_split_line(backupbuf);
        //printf("cmd:%d\n",**command);
        status = mysh_execute(command);
        
        free(buf);
        free(command);
        buf = NULL;
        command = NULL;
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
            int ret = cmd_OutPut(buf);
            return 1;
        }
    }

    //识别重定向输入
    for(int i = 0; i < MAX_CMD; i++){
        if(strcmp(command[i], "<") == 0){
            int ret = cmd_InPut(buf);
            return 1;
        }
    }

    //识别追加写重定向
    for(int i = 0; i < MAX_CMD; i++){
        if(strcmp(command[i], ">>") == 0){
            int ret = cmd_ReOutPut(buf);
            return 1;
        }
    }

    //识别管道
    for(int i = 0; i < MAX_CMD; i++){
        if(strcmp(command[i], "|") == 0){
            int ret = cmd_Pipe(buf);
            return 1;
        }
    }

    //识别后台运行
    for(int i = 0; i < MAX_CMD; i++){
        if(strcmp(command[i], "&") == 0){
            int ret = cmd_InBackground(buf);
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

int mysh_cd(char **command){
    if(command[1] == NULL) {
        perror("Mysh error at cd, lack of args.\n");
        return 1;
    }
    
    if(strcmp(command[1], "-") != 0 && strcmp(command[0], "~") != 0){
        char cd_buf[BUFFSIZE];
        getcwd(cd_buf, BUFFSIZE);
        strcpy(cdform[++cd_num], cd_buf);
    }


    if(strcmp(command[1], "~") == 0){
        strcpy(command[1], "/home/lin/");
    }

    if(strcmp(command[1], "-") == 0){
        if(cd_num > 0){
            strcpy(command[1], cdform[cd_num]);
        }else {
            strcpy(command[1], cdform[0]);
        }
        cd_num--;
    }

    if(chdir(command[1]) != 0)
        perror("Mysh error at chdir.\n");

    return 1;
}

int mysh_help(char **command){
    puts("This is Lin's shell.");
    puts("Here are some built in cmd:");
    for (int i = 0; i < mysh_builtin_nums(); i++)
        printf("%s\n", builtin_cmd[i]);

    return 1;
}

//待测试,修改
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
    char OutFile[BUFFSIZE];
    memset(OutFile, 0x00, BUFFSIZE);
    int num = 0;     //统计重定向符号的数量
    for(int i = 0; i + 1 < strlen(buf); i++){
        if(buf[i] == '>' && buf[i + 1] == ' '){
            num++;
            break;
        }
    }
    if(num != 1){
        printf("输出重定向指令输入错误\n");
        //printf("num:%d\n",num);
        return 0;
    }
    for(int i = 0; i < argc; i++){
        if(strcmp(command[i], ">") == 0){
            if(i + 1 < argc){
                strcpy(OutFile, command[i + 1]);
            }else{
                printf("缺少输出文件\n");
                return 0;
            }
        }
    }

    int index;
    for(int j = 0; j < strlen(buf); j++){
        if(buf[j] == '>'){
            index = j;
            break;
        }
    }
    buf[index] = '\0';
    buf[index + 1] = '\0';
    parse(buf); //处理符号前的指令

    pid_t pid,wpid;
    int status;

    pid = fork();
    if(pid == -1)
        perror("Mysh error at OutPut_fork.\n");
    else if(pid == 0)
    {
        int fd;
        fd = open(OutFile, O_WRONLY | O_CREAT | O_TRUNC, 7777);
        //文件打开失败
        if(fd == -1)
            exit(1);
        dup2(fd, STDOUT_FILENO);
        execvp(argv[0], argv);
        if(fd != STDOUT_FILENO){
            close(fd);
        }
        printf("%s:error.\n",argv[0]);
        exit(1);
    }
    else {
        do
        {
            wpid = waitpid(pid, &status, WUNTRACED);
        }while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

int cmd_InPut(char *buf){
    char InFile[BUFFSIZE];
    memset(InFile, 0x00, BUFFSIZE);
    int num = 0;     //统计重定向符号的数量
    for(int i = 0; i + 1 < strlen(buf); i++){
        if(buf[i] == '<' && buf[i + 1] == ' '){
            num++;
            break;
        }
    }
    if(num != 1){
        printf("输入重定向指令输入错误\n");
        return 0;
    }
    for(int i = 0; i < argc; i++){
        if(strcmp(command[i], "<") == 0){
            if(i + 1 < argc){
                strcpy(InFile, command[i + 1]);
            }else{
                printf("缺少输入指令\n");
                return 0;
            }
        }
    }

    int index;
    for(int j = 0; j < strlen(buf); j++){
        if(buf[j] == '<'){
            index = j;
            break;
        }
    }
    buf[index] = '\0';
    buf[index + 1] = '\0';
    parse(buf); //处理符号前的指令

    pid_t pid,wpid;
    int status;

    pid = fork();
    if(pid == -1)
        perror("Mysh error at InPut_fork.\n");
    else if(pid == 0)
    {
        int fd;
        fd = open(InFile, O_RDONLY, 7777);
        //文件打开失败
        if(fd == -1)
            exit(1);
        dup2(fd, STDIN_FILENO);
        execvp(argv[0], argv);
        if(fd != STDIN_FILENO){
            close(fd);
        }
        printf("%s:error.\n",argv[0]);
        exit(1);
    }
    else {
        do
        {
            wpid = waitpid(pid, &status, WUNTRACED);
        }while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

int cmd_ReOutPut(char *buf){
    char ReOutFile[BUFFSIZE];
    memset(ReOutFile, 0x00, BUFFSIZE);
    int num = 0;    //统计重定向符号的数量
    for(int i = 0; i + 2 < strlen(buf); i++){
        if(buf[i] == '>' && buf[i + 1] == '>' && buf[i + 2] == ' '){
            num++;
            break;
        }
    }
    if(num != 1){
        printf("追加输出重定向指令输入错误\n");
        return 0;
    }
    for(int i = 0; i < argc; i++){
        if(strcmp(command[i], ">>") == 0){
            if(i + 1 < argc){
                strcpy(ReOutFile, command[i + 1]);
            }else{
                printf("缺少输出文件\n");
                return 0;
            }
        }
    }

    int index;
    for(int j = 0; j + 2 < strlen(buf); j++){
        if(buf[j] == '>' && buf[j + 1] == '>' && buf[j + 2] == ' '){
            index = j;
            break;
        }
    }
    buf[index] = '\0';
    buf[index + 1] = '\0';
    parse(buf); //处理符号前的指令

    pid_t pid,wpid;
    int status;

    pid = fork();
    if(pid == -1)
        perror("Mysh error at ReOutPut_fork.\n");
    else if(pid == 0)
    {
        int fd;
        fd = open(ReOutFile, O_WRONLY | O_CREAT | O_APPEND, 7777);
        //文件打开失败
        if(fd == -1)
            exit(1);
        dup2(fd, STDOUT_FILENO);
        execvp(argv[0], argv);
        if(fd != STDOUT_FILENO){
            close(fd);
        }
        printf("%s:error.\n",argv[0]);
        exit(1);
    }
    else {
        do
        {
            wpid = waitpid(pid, &status, WUNTRACED);
        }while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

int cmd_Pipe(char *buf){
    int j, len;
    for (j = 0; j < strlen(buf); j++){
        if(buf[j] == '|')
            break;
    }

    len = j;
    char outputBuf[len];
    memset(outputBuf, 0x00, len);
    char inputBuf[strlen(buf) - len];
    memset(inputBuf, 0x00, strlen(buf) - len);
    for(int i = 0; i < len - 1; i++){
        outputBuf[i] = buf[i];
    }
    for(int i = 0; i < strlen(buf) - len - 1; i++){
        inputBuf[i] = buf[len + 2 + i];
    }

    int pipefd[2];
    pid_t pid, wpid;
    int status;
    if(pipe(pipefd) == -1){
        perror("Mysh error at pipe.\n");
        exit(1);
    }

    pid = fork();
    if(pid == -1){
        perror("Mysh error at pipe_fork.\n");
    }
    else if(pid == 0){
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        parse(outputBuf);
        execvp(argv[0], argv);
        if(pipefd[1] != STDOUT_FILENO){
            close(pipefd[1]);
        }
    }
    else {
        do
        {
            wpid = waitpid(pid, &status, WUNTRACED);
        }while(!WIFEXITED(status) && !WIFSIGNALED(status));

        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        parse(inputBuf);
        execvp(argv[0], argv);
        if(pipefd[0] != STDIN_FILENO){
            close(pipefd[0]);
        }
    }
    return 1;

}

int cmd_InBackground(char *buf){
    char backgroundBuf[strlen(buf)];
    memset(backgroundBuf, 0x00, strlen(buf));
    //提取&前的指令并做处理
    for (int i = 0; i < strlen(buf); i++){
        backgroundBuf[i] = buf[i];
        if(buf[i] == '&'){
            backgroundBuf[i] = '\0';
            backgroundBuf[i - 1] = '\0';
            break;
        }
    }
    pid_t pid, wpid;
    int status;
    pid = fork();
    if(pid == -1){
        perror("Mysh error at InBackground_fork.\n");
    }
    else if(pid == 0){
        freopen("/dev/null", "w", stdout);
        freopen("/dev/null", "r", stdin);
        signal(SIGCHLD, SIG_IGN);
        //子进程调用该函数，让Linux接管该子进程
        parse(backgroundBuf);
        execvp(argv[0], argv);
        printf("%s:error.\n", argv[0]);
        exit(1);
    }else {
        //父进程不需要等待子进程就可以返回
        exit(0);
    }

}