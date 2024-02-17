#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/types.h>
#include <string.h>

#define PATH_MAX 4096

void do_ls(char *dirname);
void print_file_info(char *filename,char *pathname);
void mode_change(int mode, char* str);
char *uid_to_name(uid_t uid);
char *gid_to_name(gid_t gid);
void get_color(struct stat file,char *name);
void sort_name(char **filenames, int cnt);
void sort_time(mode_t *filetime,int cnt);

int has_a = 0;
int has_i = 0;
int has_l = 0;
int has_s = 0;
int has_t = 0;
int has_r = 0;
int has_R = 0;
char **filenames;
//char filenames[10000][PATH_MAX + 1];
mode_t *filetime;

int main(int argc,char *argv[])
{
    int Flag = 0;
    filenames = (char **)malloc(sizeof(char *) * 10000);
    for(int i = 0;i < 10000; i++){
        filenames[i] = (char *)malloc(sizeof(char) * (PATH_MAX + 1));
    }
    filetime = (mode_t *)malloc(sizeof(mode_t) * 10000);
    
    for(int i = 1; i < argc; i++){  //统计指令
        if(argv[i][0] == '-'){
            for(int j = 1; j < strlen(argv[i]); j++){
                if(argv[i][j] == 'a') 
                    has_a++;
                else if (argv[i][j] == 'i')
                    has_i++;
                else if (argv[i][j] == 'l')
                    has_l++;
                else if (argv[i][j] == 'r')
                    has_r++;
                else if (argv[i][j] == 't')
                    has_t++;
                else if (argv[i][j] == 's')
                    has_s++;
                else if (argv[i][j] == 'R')
                    has_R++;
                else{
                    perror("ls: 不适用的选项 \n请尝试执行 \"ls --help\" 来获取更多信息。");
                    return 0;
                }
            }
        }
    }
    
    for(int i = 1; i < argc; i++){
        if(argv[i][0] != '-'){
            Flag = 1;
            printf("%s:\n",argv[i]);
            do_ls(argv[i]);   //处理每一个目录
            printf("\n");
        }
    }
    if(Flag == 0){
        do_ls("."); //默认使用当前目录
    }

    for(int i = 0; i < 10000; i++) {
        free(filenames[i]);
    }
    free(filenames);
    free(filetime);
    printf("\n");
    return 0;
}

void do_ls(char *dirname){
    int file_cnt = 0;
    int flag = 0;
    DIR *dir_ptr = NULL;
    char pathname[256];
    struct dirent *direntp = NULL;
    struct stat info;

    if((dir_ptr = opendir(dirname)) == NULL){
        fprintf(stderr,"ls:cannot open :%s\n",dirname);
    }
    else{
        //记录文件信息
        while((direntp = readdir(dir_ptr)) != NULL){
            filenames[file_cnt++] = direntp->d_name;
            sprintf(pathname,"%s/%s",dirname,filenames[file_cnt - 1]);
            if(stat(pathname, &info) == -1){
                perror(pathname);
                exit(EXIT_FAILURE);
            }
            else {
                filetime[file_cnt - 1] = info.st_mtime;
            }
        }

        sort_name(filenames,file_cnt);
            
        if(has_t) {
            sort_time(filetime,file_cnt);
        }

        for(int i = 0; i < file_cnt; i++){
            if(filenames[i][0] == '.' && has_a == 0)
                continue;
            sprintf(pathname,"%s/%s",dirname,filenames[i]);
            if(stat(pathname, &info) == -1){
                perror(pathname);
                exit(EXIT_FAILURE);
            }
            else {
                if(has_i){
                    printf("%lu ",info.st_ino); 
                }

                if(has_s){
                    long long size = info.st_size/1024;
                    if(size <= 4)
                        printf("%-4d",4);
                    else 
                        printf("%-4lld",size);
                }

                if(has_l) {
                    print_file_info(filenames[i],pathname);
                }
                else {
                    get_color(info,filenames[i]);
                }

                if(has_R){
                    if(S_ISDIR(info.st_mode)){
                        if(strcmp(filenames[i],".") !=0 && strcmp(filenames[i],"..") != 0){
                            sprintf(pathname,"%s/%s",dirname,filenames[i]);
                            printf("%s:\n",pathname);
                            do_ls(pathname);
                        }
                    }
                }
            }
        }
    }
    closedir(dir_ptr);
}

void print_file_info(char *filename,char *pathname){  
    struct stat file_stat;
    if(stat(pathname, &file_stat) == -1){
        perror(pathname);
        exit(EXIT_FAILURE);
    }
    else {
        void mode_change();
        char *uid_to_name(), *gid_to_name(), *ctime();
        struct stat *file_stat_p = &file_stat;
        char modestr[11];
        mode_change(file_stat_p->st_mode,modestr);
        printf("%s ",modestr);
        printf("%3d ",(int)file_stat_p->st_nlink);
        printf("%7s ",uid_to_name(file_stat_p->st_uid));
        printf("%7s ",gid_to_name(file_stat_p->st_gid));
        printf("%10ld ",(long)file_stat_p->st_size);
        printf("%.14s ",4 + ctime(&file_stat_p->st_mtime));
        get_color(file_stat,filename);
        printf("\n");
    }
}

void mode_change(int mode, char *str){
    strcpy(str,"----------");
    if(S_ISDIR(mode))  
        str[0] = 'd';  //directory
    if(S_ISCHR(mode))  
        str[0] = 'c';  //char device
    if(S_ISBLK(mode))  
        str[0] = 'b';  //block device

    //user
    if(mode & S_IRUSR) 
        str[1] = 'r';
    if(mode & S_IWUSR) 
        str[2] = 'w';
    if(mode & S_IXUSR) 
        str[3] = 'x';

    //group
    if(mode & S_IRGRP) 
        str[4] = 'r';
    if(mode & S_IWGRP) 
        str[5] = 'w';
    if(mode & S_IXGRP) 
        str[6] = 'x';
    
    //other
    if(mode & S_IROTH) 
        str[7] = 'r';
    if(mode & S_IWOTH) 
        str[8] = 'w';
    if(mode & S_IXOTH) 
        str[9] = 'x';
}

char *uid_to_name(uid_t uid){
    struct passwd *getpwuid();
    struct passwd *user_p;
    static char namestr[10];
    user_p = getpwuid(uid);
    if(user_p == NULL){
        sprintf(namestr,"%d",uid);
        return namestr;
    }
    else {
        return user_p->pw_name;
    }
}

char *gid_to_name(gid_t gid){
    struct passwd *getpwuid();
    struct passwd *group_p;
    static char namestr[10];
    group_p = getpwuid(gid);
    if(group_p == NULL){
        sprintf(namestr,"%d",gid);
        return namestr;
    }
    else {
        return group_p->pw_name;
    }
}

void get_color(struct stat file,char *name){
    if(S_ISLNK(file.st_mode)){  //符号链接
        printf("\033[01;36m%-20s\033[0m",name); //加粗深绿
    }
    else if(S_ISDIR(file.st_mode)){  //目录
        printf("\033[01;34m%-20s\033[0m",name);   //加粗蓝
    }
    else if(S_ISCHR(file.st_mode) || S_ISBLK(file.st_mode)){ //块设备和字符设备
        printf("\033[40;33m%-20s\033[0m",name);   //背景黑字体黄
    }
    else if(S_ISFIFO(file.st_mode)){  //FIFO或管道
        printf("\033[01;33m%-20s\033[0m",name);  //加粗黄
    }
    else if(S_ISSOCK(file.st_mode)){   //套接字
        printf("\033[01;35m%-20s\033[0m",name);  //加粗紫色
    }
    else if(S_ISREG(file.st_mode)){  //常规文件，并且有可执行权限
        if(file.st_mode & S_IXUSR || file.st_mode & S_IXGRP || file.st_mode & S_IXOTH){
            printf("\033[01;32m%-20s\033[0m",name);  //加粗绿
        }
        else 
            printf("%-20s",name);
    }
    else 
        printf("%-20s",name);

    if(!has_l){
        static int flag = 0;
        flag++;
        if(flag == 4){
            printf("\n");
            flag = 0;
        }
    }
}

void sort_name(char **filenames,int cnt){
    char temp[PATH_MAX];
    if(has_r)
    {
        for(int i = 0; i < cnt - 1; i++) {
            for(int j = 0; j < cnt - 1 - i; j++) {
                if(strcmp(filenames[j],filenames[j+1]) < 0) {
                    strcpy(temp,filenames[j]);
                    strcpy(filenames[j],filenames[j+1]);
                    strcpy(filenames[j+1],temp);
                }
            }
        }
    }
    else {
        for(int i = 0; i < cnt - 1; i++) {
            for(int j = 0; j < cnt - 1 - i; j++) {
                if(strcmp(filenames[j],filenames[j+1]) > 0) {
                    strcpy(temp,filenames[j]);
                    strcpy(filenames[j],filenames[j+1]);
                    strcpy(filenames[j+1],temp);
                }
            }
        }
    }
}

void sort_time(mode_t *filetime,int cnt){
    mode_t timetemp;
    char temp[PATH_MAX];
    for(int i = 0; i < cnt - 1; i++) {
        for(int j = 0; j < cnt - 1 - i; j++) {
            if(filetime[j] < filetime[j+1]) {
                timetemp = filetime[j];
                filetime[j] = filetime[j+1];
                filetime[j+1] = timetemp;
                strcpy(temp,filenames[j]);
                strcpy(filenames[j],filenames[j+1]);
                strcpy(filenames[j+1],temp);
            }
        }
    }
}