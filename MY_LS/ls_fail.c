#include "my_ls.h"


int main(int argc,char *argv[])
{
    int Flag = 0;
    int opt;
    
    while ((opt = getopt(argc, argv, "alRtris")) != -1)
    {
        switch (opt)
        {
            case 'a':      
                has_a = 1;
                break;
            case 'l':
                has_l = 1;
                break;
            case 'R':
                has_R = 1; 
                break;
            case 't':
                has_t = 1;
                break;
            case 'r':
                has_r = 1;
                break;
            case 'i':
                has_i = 1;
                break;
            case 's':
                has_s = 1;
                break;
            case '?':
                printf("ls: 不适用的选项 \n请尝试执行 \"ls --help\" 来获取更多信息。\n");
                break;
        }     
    }
    
    for(int i = 1; i < argc; i++){
        if(argv[i][0] != '-'){
            Flag = 1;
            printf("%s:\n",argv[i]);
            char path[256];
            realpath(argv[i],path);
            do_ls(path);   //处理每一个目录
            printf("\n");
        }
    }
    if(Flag == 0){
        do_ls("."); //默认使用当前目录
    }
    printf("\n");
    return 0;
}

void do_ls(char *dirname){
    int File_cnt = 0;
    int flag = 0;
    char filepath[1024];
    char real_path[1024];
    DIR *dir_ptr = NULL;
    struct dirent *direntp = NULL;
    struct stat info;

    if((dir_ptr = opendir(dirname)) == NULL){
        fprintf(stderr,"ls:cannot open :%s:权限不够\n",dirname);
    }
    else{
        //拼接路径,记录文件数量
        while((direntp = readdir(dir_ptr)) != NULL){
            if (direntp->d_name[0] == '.' && has_a == 0)
                continue;
            snprintf(filepath, sizeof(filepath), "%s/%s", dirname, direntp->d_name);
            if(access(filepath,R_OK) == -1)
                continue;//检查访问权限
            
            if(lstat(filepath, &info) == -1){
                perror(filepath);
                exit(EXIT_FAILURE);
            }
            else
                File_cnt++;          
            }
        }

    //分配内存
    File *fileinfo = NULL;
    fileinfo = (File *)malloc(sizeof(File) * File_cnt);
    printf("malloc : %d\n",File_cnt);
    printf("mallocsize : %ld\n",sizeof(*fileinfo));
    rewinddir(dir_ptr);

    //记录文件信息
    int j = 0;
    while((direntp = readdir(dir_ptr)) != NULL){
        if (direntp->d_name[0] == '.' && has_a == 0)
            continue;        
        strcpy(fileinfo[j].filename, direntp->d_name);
        snprintf(filepath, sizeof(filepath), "%s/%s", dirname, direntp->d_name);

        if(access(filepath,R_OK) == -1){
            //printf("无权限访问:%s\n",filepath);
            continue;//检查访问权限
            }

        if(lstat(filepath, &fileinfo[j].info) == -1){
            perror(filepath);
            exit(EXIT_FAILURE);
        }
            j++;
    }

    if(has_t)
        qsort(fileinfo, File_cnt, sizeof(File), compare_t);
    else
        qsort(fileinfo, File_cnt, sizeof(File), compare);
    
    if(has_r) {
        for(int i = File_cnt - 1; i >= 0; i--)
        {
            if(has_i){
                printf("%7lu ",fileinfo[i].info.st_ino); 
            }

            if(has_s){
                long long size = fileinfo[i].info.st_size/1024;
                if(size <= 4)
                    printf("%-4d",4);
                else 
                    printf("%-4lld",size);
            }

            if(has_l) 
                print_file_info(fileinfo[i]);
            else 
                get_color(fileinfo[i],fileinfo[i].filename);
        }    
    }
    else {
        for(int i = 0; i < File_cnt; i++)
        {
            if(has_i){
                printf("%7lu ",fileinfo[i].info.st_ino); 
            }

            if(has_s){
                long long size = fileinfo[i].info.st_size/1024;
                if(size <= 4)
                    printf("%-4d",4);
                else 
                    printf("%-4lld",size);
            }

            if(has_l) 
                print_file_info(fileinfo[i]);
            else 
                get_color(fileinfo[i],fileinfo[i].filename);
        }
    }

    rewinddir(dir_ptr);

    if(has_R){
        static int num;
        printf("num:%d\n",num++);
        while((direntp = readdir(dir_ptr)) != NULL){
            if(has_a == 0 && direntp->d_name[0] == '.')
                continue;
            if(direntp->d_type == DT_DIR && strcmp(direntp->d_name, "..") != 0 && strcmp(direntp->d_name, ".") != 0){
                snprintf(filepath, sizeof(filepath), "%s/%s", dirname, direntp->d_name);
                /* if(access(filepath,R_OK) == -1)
                    continue;//检查访问权限 */
                realpath(filepath,real_path);
                printf("\n%s:\n",real_path);
                do_ls(real_path);
            }
        }
    }

    closedir(dir_ptr);

    free(fileinfo);
    fileinfo = NULL;
    printf("free : %ld\n",sizeof(*fileinfo));
}

void print_file_info(File fileinfo){  
    void mode_change();
    char *uid_to_name(), *gid_to_name(), *ctime();
    struct stat *file_stat_p = &fileinfo.info;
    char modestr[11];
    mode_change(file_stat_p->st_mode,modestr);

    printf("%s ",modestr);
    printf("%4d ",(int)file_stat_p->st_nlink);
    printf("%-8s ",uid_to_name(file_stat_p->st_uid));
    printf("%-8s ",gid_to_name(file_stat_p->st_gid));
    printf("%8ld ",(long)file_stat_p->st_size);
    printf("%.12s ",4 + ctime(&file_stat_p->st_mtime));
    get_color(fileinfo,fileinfo.filename);
    printf("\n");
}

void mode_change(int mode, char *str){
    strcpy(str,"----------");
    switch (mode & __S_IFMT)
    {
    case __S_IFREG:
        str[0] = '-';
        break;
    case __S_IFDIR:
        str[0] = 'd';
        break;
    case __S_IFCHR:
        str[0] = 'c';
        break;
    case __S_IFBLK:
        str[0] = 'b';
        break;
    case __S_IFIFO:
        str[0] = 'p';
        break;
    case __S_IFSOCK:
        str[0] = 's';
        break;
    case __S_IFLNK:
        str[0] = 'l';
        break;
    }

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
    str[10] = '\0';
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

void get_color(File fileinfo,char *name){
    mode_t mode = fileinfo.info.st_mode;

    if(S_ISLNK(mode)){  //符号链接
        printf("\033[01;36m%-20s\033[0m",name); //加粗深绿
    }
    else if(S_ISDIR(mode)){  //目录
        printf("\033[01;34m%-20s\033[0m",name);   //加粗蓝
    }
    else if(S_ISCHR(mode) || S_ISBLK(mode)){ //块设备和字符设备
        printf("\033[40;33m%-20s\033[0m",name);   //背景黑字体黄
    }
    else if(S_ISFIFO(mode)){  //FIFO或管道
        printf("\033[01;33m%-20s\033[0m",name);  //加粗黄
    }
    else if(S_ISSOCK(mode)){   //套接字
        printf("\033[01;35m%-20s\033[0m",name);  //加粗紫色
    }
    else if(S_ISREG(mode)){  //常规文件，并且有可执行权限
        if(mode & S_IXUSR || mode & S_IXGRP || mode & S_IXOTH){
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

int compare(const void *a, const void *b)
{
    File *a_ = (File *)a;
    File *b_ = (File *)b;
    return strcmp(a_->filename,b_->filename);
}

int compare_t(const void *a, const void *b)
{
    File *a_ = (File *)a;
    File *b_ = (File *)b;
    return a_->info.st_mtime < b_->info.st_mtime;
}