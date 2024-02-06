#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<sys/stat.h>
#include<string.h>
#define MAX_LEN 150

/*struct dirent{
    ino_t          d_ino; //i号节点
    off_t          d_off;
    unsigned short d_reclen; //记录信息长度
    unsigned char  d_type; //文件类型，非所有文件都支持
    char           d_name[256]; //文件名称
}
*/

void do_ls(char *dirname);

int main(int argc,char *argv[])
{
    if(argc == 1){
        do_ls("."); //默认使用当前目录
    }
    else{
        while(--argc){
            printf("%s:\n",*++argv);
            do_ls(*argv);
        }
    }
    return 0;
}

void do_ls(char *dirname){
    DIR *dir_ptr = NULL;
    struct dirent *direntp = NULL;

    if((dir_ptr = opendir(dirname)) == NULL){
        fprintf(stderr,"ls:cannot open :%s\n",dirname);
    }
    else{
        while((direntp = readdir(dir_ptr)) != NULL){
            printf("%s\n",direntp->d_name);
        }
        closedir(dir_ptr);
    }
}

