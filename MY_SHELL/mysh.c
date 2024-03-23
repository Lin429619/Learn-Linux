#include "mysh.h"

char * builtin_cmd[] = 
{
    "cd",
    "help",
    "exit"
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

void getUsername() { // 获取当前登录的用户名
	struct passwd* pwd = getpwuid(getuid());
	strcpy(username, pwd->pw_name);
}

void getHostname() { // 获取主机名
	gethostname(hostname, BUFFSIZE);
}

int getCurWorkDir() { // 获取当前的工作目录
	char* result = getcwd(curPath, BUFFSIZE);
	if (result == NULL)
		return 1;
	else return 0;
}

int main(int argc, char *argv[])
{
    int result = getCurWorkDir();
	if (1 == result) {
		fprintf(stderr, "\e[31;1mError: System error while getting current work directory.\n\e[0m");
		exit(1);
	}
	getUsername();
	getHostname();

    strcpy(cdform[0], curPath);
    signal(SIGINT, SIG_IGN);  //屏蔽ctrl+c

    int status;

    do
    {
        result = getCurWorkDir();
        printf("\e[32;1m%s@%s:%s\e[0m$ ", username, hostname,curPath); // 显示为绿色
        
        buf = mysh_read_line();

        command = mysh_split_line(backupbuf);
        //printf("cmd:%s\n",command[0]);
        result = mysh_execute(command);
        //printf("status:%d\n",status);
        
        free(buf);
        free(command);
        buf = NULL;
        command = NULL;
    } while (status);
    
    return 0;
}

int mysh_execute(char **command)
{
    if(command == NULL || command[0] == NULL) return 1;

    //先识别重定向，管道等指令
    pid_t pid, wpid;
    int status;

    pid = fork();
    if(pid == -1)
        perror("Mysh error at fork.\n");
    else if(pid == 0)
    {
        //获取标准输入、输出的文件标识符
		int inFds = dup(STDIN_FILENO);
		int outFds = dup(STDOUT_FILENO);

        int ret = cmd_WithPipe(0, argc);

		//还原标准输入、输出重定向 
		dup2(inFds, STDIN_FILENO);
		dup2(outFds, STDOUT_FILENO);
		exit(ret);
    }
    else {
        do
        {
            wpid = waitpid(pid, &status, WUNTRACED);
        }while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    //识别后台运行
    for(int i = 0; i < MAX_CMD && command[i] != NULL; i++){
        if(strcmp(command[i], "&") == 0){
            int ret = cmd_InBackground(command, i);
            return 1;
        }
    }

    //识别内置命令
    for (int i = 0; i < mysh_builtin_nums(); i++){
        if(strcmp(command[0], builtin_cmd[i]) == 0)
            return (*builtin_func[i])(command);
    }
    
    return 1;
}

int mysh_builtin_nums(){
    return sizeof(builtin_cmd) / sizeof(builtin_cmd[0]);
}

int mysh_cd(char **command){  //处理cd- cd~ 和普通的cd命令
    if(command[1] == NULL) {
        perror("Mysh error at cd, lack of args.\n");
        return 1;
    }
    
    /* if(strcmp(command[1], "-") != 0 && strcmp(command[1], "~") != 0){
        char cd_buf[BUFFSIZE];
        getcwd(cd_buf, BUFFSIZE);
        strcpy(cdform[++cd_num], cd_buf);
    } */
    char cd_buf[BUFFSIZE];
    getcwd(cd_buf, BUFFSIZE);
    strcpy(cdform[++cd_num], cd_buf);

    char path[BUFFSIZE];
    strcpy(path, "/home/");
    strcat(path, username);
    strcat(path, "/");
    if(strcmp(command[1], "~") == 0){
        strcpy(command[1], path);
    }

    if(strcmp(command[1], "-") == 0){
        if(--cd_num > 0){
            strcpy(command[1], cdform[cd_num]);
        }else {
            strcpy(command[1], cdform[0]);
        }
        printf("%s\n",command[1]);
        //cd_num--;
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

int mysh_exit(char **command){
    pid_t pid = getpid();
	kill(pid, SIGTERM);
}

int cmd_WithPipe(int left ,int right)
{
    //判断是否有管道命令
    int pipeIdx = -1;
	for (int i = left; i < right; i++) {
		if (strcmp(command[i], "|") == 0) {
			pipeIdx = i;
			break;
		}
	}
	if (pipeIdx == -1) { // 不含有管道命令
		return cmd_WithRedi(left, right);
	} else if (pipeIdx + 1 == right) { // 管道命令'|'后续没有指令，参数缺失
		printf("管道缺少后续指令\n");
        return 1;   
	}

    int fds[2];
    pid_t pid, wpid;
    int status;
	if (pipe(fds) == -1) {
		perror("Mysh error at pipe.\n");
        return 1;
	}

	pid = fork();
	if (pid == -1) {
		perror("Mysh error at pipe_fork.\n");
        exit(1);
	} 
    else if (pid == 0) {  //子进程执行单个命令
		close(fds[0]);
		dup2(fds[1], STDOUT_FILENO);  //将标准输出重定向到fds[1]
		close(fds[1]);
		
		int result = cmd_WithRedi(left, pipeIdx);
        if (result == 1) { //执行失败
            perror("Failed to execute command before pipe.\n");
            exit(1);
        }
	} 
    else {  //父进程递归执行后续命令
        do{
            wpid = waitpid(pid, &status, WUNTRACED);
        }while(!WIFEXITED(status) && !WIFSIGNALED(status));

		if(pipeIdx + 1 < right){
			close(fds[1]);
			dup2(fds[0], STDIN_FILENO);  //将标准输入重定向到fds[0]
			close(fds[0]);
			int result = cmd_WithPipe(pipeIdx + 1, right);  //递归执行后续指令
            if (result == 1) { //执行失败
                perror("Failed to execute command after pipe.\n");
                exit(1);
            }
		}        
	}

    return 0;
}

int cmd_WithRedi(int left ,int right)
{
    //判断是否存在重定向和追加命令
    int inNum = 0, outNum = 0, reoutNum = 0;
	char *inFile = NULL, *outFile = NULL, *reoutFile = NULL;
	int endIdx = right; // 指令在重定向前的终止下标

    for (int i = left; i < right; i++) {
		if (strcmp(command[i], "<") == 0) { // 输入重定向
			inNum++;
			if (i + 1 < right)
				inFile = command[i + 1];
			else {
                printf("缺少输入文件\n");
                return 1;   // 重定向符号后缺少文件名
            }

			if (endIdx == right) endIdx = i;
		} else if (strcmp(command[i], ">") == 0) { // 输出重定向
			outNum++;
			if (i + 1 < right)
				outFile = command[i + 1];
			else {
                printf("缺少输出文件\n");
                return 1;   // 重定向符号后缺少文件名
            } 
				
			if (endIdx == right) endIdx = i;
		}else if (strcmp(command[i], ">>") == 0) { // 追加
			reoutNum++;
			if (i + 1 < right)
				reoutFile = command[i + 1];
			else {
                printf("缺少输出文件\n");
                return 1;   // 重定向符号后缺少文件名
            }
				
			if (endIdx == right) endIdx = i;
        }
	}

    //处理各项命令
    if(inNum > 1){
        printf("输入重定向指令输入错误\n");
        return 1;
    }else if (outNum > 1){
        printf("输出重定向指令输入错误\n");
        return 1;
    }else if (reoutNum > 1){
        printf("追加输出重定向指令输入错误\n");
        return 1;
    }

    //提取有效命令
    char* argv[BUFFSIZE];
	for (int i = left; i < endIdx; i++)
		argv[i] = command[i];
	argv[endIdx] = NULL;
    //printf("%s\n",argv[endIdx - 1]);

    pid_t pid,wpid;
    int status;

    pid = fork();
    if(pid == -1)
        perror("Mysh error at OutPut_fork.\n");
    else if(pid == 0){
        if(outNum){
            freopen(outFile, "w", stdout);
        }
        else if(inNum){
            freopen(inFile, "r", stdin);
        }
        else if(reoutNum){
            freopen(reoutFile, "a+", stdout);
        }
        execvp(argv[left], argv + left);
    }
    else {
            do
            {
                wpid = waitpid(pid, &status, WUNTRACED);
            }while(!WIFEXITED(status) && !WIFSIGNALED(status));
        }

    return 0;
}

int cmd_InBackground(char **command, int index){
    char* argv[BUFFSIZE];
    for(int i = 0; i < index; i++){
        argv[i] = command[i];
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
        execvp(argv[0], argv);
    }else {
        //父进程不需要等待子进程结束就可以返回
    }

}
