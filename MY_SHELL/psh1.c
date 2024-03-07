# include<stdio.h>
# include<signal.h>
# include<string.h>
# include<stdlib.h>
# include<unistd.h>
# include<sys/types.h>
# include<sys/wait.h>

#define MAXARGS 20
#define ARGLEN 100

int main()
{
    char *arglist[MAXARGS + 1];
    int numargs = 0;
    char argbuf[ARGLEN];
    char *makestring();
    int execute();
    while(numargs < MAXARGS){
        printf("Arg[%d]?", numargs);
        if(fgets(argbuf, ARGLEN, stdin) && *argbuf != '\n')
            arglist[numargs++] = makestring(argbuf); 

        else {
            if(numargs > 0){
                arglist[numargs] = NULL;
                execute(arglist);
                numargs = 0;
            }
        }
    }
    return 0;
}

int execute(char *arglist[]){

    int pid,exitstatus;
    pid = fork();
    switch(pid){
        case -1:
            perror("fork failed");
            exit(1);
        case 0: //子进程的代码
            execvp(arglist[0],arglist);
            perror("execvp failed");
            exit(1);
        default: //父进程的代码
            while(wait(&exitstatus) != pid) ;
            printf("child exited with status %d,%d\n",exitstatus >> 8,exitstatus & 0377);                
    }                                                  //退出码         //终止信号
}

char *makestring(char *buf){
    char *cp;
    buf[strlen(buf) - 1] = '\0';  //去掉输入字符串末尾的换行符或回车符
    cp = (char*)malloc(strlen(buf) + 1);
    if(cp == NULL){
        fprintf(stderr,"no memory\n");
        exit(1);
    }
    strcpy(cp, buf);
    return cp;
}