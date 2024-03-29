## 进程

##### 通过命令ps了解进程
* 与ls类似，ps也支持`-a`,`-l`可选项，`UID`列指明用户ID，每个进程都有相应的进程ID(PID)和父进程(PPID)。1号进程——init进程，是所有进程的始祖。`-f`表示格式化输出，也就是用用户名来代替UID。
* Unix系统中的内存分为系统内存和用户内存，进程存在于用户内存中，一个进程不一定会占用一段连续的内存，它也会被分成很多小块。 



##### 相关函数
> getpid() 返回调用进程的进程号

> execvp(const * file,const * argv[]),俩参数分别是要执行的文件名和字符串数组，execvp载入由file指定的程序到当前进程，然后运行它，该函数在环境变量PATH所指定的路径中查找file文件。

> fork() 创建子进程，系统内核将父进程的代码和运行到的当前位置都复制给子进程，新的进程从**fork返回的位置**开始运行，并非从头开始。错误返回-1,**正确返回0,并回到子进程**。

> wait() 父进程调用wait等待子进程结束，在子进程运行的同时父进程运行wait函数，接收到子进程调用exit的返回值之后继续运行自己的进程，wait()的返回值是调用exitd的子进程的PID，可以告知父进程子进进程时如何结束的（成功，死亡，失败）。wait函数的参数是**用来储存子进程的退出状态**（整数变量），由三个部分组成————8位记录退出值，7位记录信号序号，1位指明发生错误并产生内核映像。

```
//查看子进程的退出状态
main(){
    int child_status;
    int high_8,low_7,bit_7;

    wait(&child_status);
    high_8 = child_status >> 8; //exit value
    low_7 = child_status & 0x7F; //signal number
    bit_7 = child_status & 0x80; //core dump flag
}
```
> exit()和_exit() 
> 系统函数_exit()是一个内核操作，系统调用它终止当前进程并执行所有必须的清理工作。具体如下：
> (1)关闭所有文件描述符和目录描述符。
> (2)将该进程的PID置为init进程的PID。
> (3)如果父进程调用wait或waitpid来等待子进程结束，则通知父进程。
> (4)向父进程发送SIGCHLD，就算父进程没有调用wait，内核也会向它发送SIGCHLD消息。
> **注意：**如果父进程在子进程之前退出，子进程能够继续运行，成为init进程的“子女”。

##### 文件描述符

*  0:stdin（标准输入）   1:stdout（标准输出）   2:stderr（标准错误输出）
*  文件描述符是什么？  它是一个数组的索引号，每个进程都有其打开的一组文件，这些文件被保存在一个数组中，文件描述符即为这些文件的索引。进程请求新的文件描述符时，内核就将**最低可用的文件描述符**赋给它。

|命令	         |  说明          |
--------        | --------
command > file	| 将输出重定向到 file
command < file	| 将输入重定向到 file
command >> file	| 将输出以追加的方式重定向到 file
n> file	        | 将文件描述符为 n 的文件重定向到 file
n>> file	    | 将文件描述符为 n 的文件以追加的方式重定向到 file
n>&m	        | 将输出文件 m 和 n 合并，如2>&1，输出和错误重定向到同一个文件
n<&m	        | 将输入文件 m 和 n 合并

> * getcwd()函数
> 函数说明：`getcwd` 方法会将当前工作目录的绝对路径复制到参数 `buffer` 所指的内存空间中，而参数` size `是 `buffer` 所指的空间大小。

第一种基本使用方法：
```
#define MAX_SIZE 255
int main(int argc, const char* argv[]){
    char path[MAX_SIZE];
    getcwd(path,sizeof(path));
    puts(path); 
    return 0;
} //存在弊端：当工作目录绝对路径长于所指定的 size 时，则会返回 NULL。
```
第二种方法：
```
int main(int argc, const char* argv[]){
    char* path;
    path = getcwd(NULL, 0);
    puts(path);
    free(path);
    return 0;
} //getcwd会调用malloc动态分配空间，注意使用完释放。
```
