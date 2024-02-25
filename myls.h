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
#include <getopt.h>

//#define PATH_MAX 1024

typedef struct {
    char filename[PATH_MAX];
    struct stat info;
}File;

void do_ls(char *dirname);
void print_file_info(File fileinfo);
void mode_change(int mode, char* str);
char *uid_to_name(uid_t uid);
char *gid_to_name(gid_t gid);
void get_color(File fileinfo,char *name);
int compare(const void *a, const void *b);
int compare_t(const void *a, const void *b);

int has_a = 0;
int has_i = 0;
int has_l = 0;
int has_s = 0;
int has_t = 0;
int has_r = 0;
int has_R = 0;