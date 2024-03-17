#include "mysh.h"

#define TOK_DELIM " \t\r\n"

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
    &mysh_help
};

int mysh_builtin_nums(){
    return sizeof(builtin_cmd) / sizeof(builtin_cmd[0]);
}

int main(int argc, char *argv[])
{
    char **args;

    do
    {
        char path[256];
        getcwd(path, 256);
        char now[300] = "[myshell ";
        strcat(now, path);
        strcat(now, " ]$");
        printf("%s",now);

        args = mysh_split_line();

        free(args);
    }while(1);

    return 0;
}

char ** mysh_split_line(){
    char *line = NULL;
    ssize_t bufsize = 0;
    getline(&line, &bufsize, stdin);

    int buffer_size = 100, position = 0;
    char **tokens = malloc(buffer_size * sizeof(char *));
    char *token;
    token = strtok(line, TOK_DELIM);
    while(token != NULL){
        tokens[position++] = token;
        token = strtok(NULL, TOK_DELIM);
    }
    tokens[position] = NULL;
    free(line);
    return tokens;
}

int mysh_cd(char **args){
    if(args[1] == NULL) {
        perror("Mysh error at cd, lack of args.\n");
    }
    else{
        if(chdir(args[1]) != 0)
            perror("Mysh error at chdir.\n");
    }
    return 1;
}

int mysh_help(char **args){
    puts("This is Lin's shell.");
    puts("Here are some built in cmd:");
    for (int i = 0; i < mysh_builtin_nums(); i++)
        printf("%s\n", builtin_cmd[i]);
    return 1;
}

int mysh_exit(char **args){
    return 0;
}

int mysh_launch(char **args){
    pid_t pid, wpid;
    int status;

    pid = fork();
    if(pid == -1)
        perror("Mysh error at fork.\n");
    else if(pid == 0)
    {
        if(execvp(args[0], args) == -1)
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

int mysh_execute(char **args){
    if(args[0] == NULL)
        return 0;
    
    //执行内置命令
    for(int i = 0; i < mysh_builtin_nums(); i++){
            if(strcmp(args[0], builtin_cmd[i]) == 0)
                return (*builtin_func[i])(args);
        }
    return mysh_launch(args);
}