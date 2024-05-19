#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
 
const int NUMBER = 2;

//任务结构体
typedef struct Task{
    void (*function)(void(* arg));
    void* arg;
}Task;

//线程池结构体
struct ThreadPool{
    Task* taskQ;
    int queueCapacity;  //容量
    int queueSize;      //当前任务个数
    int queueFront;     //队列头部
    int queueRear;      //队列尾部

    pthread_t managerID;  //管理者线程ID
    pthread_t *threadIDs; //工作线程ID
    int minNum;           //最小线程数
    int maxNum;           //最大线程数
    int busyNum;          //工作中的线程数
    int liveNum;          //存活的线程数
    int exitNum;          //待销毁的线程数
    pthread_mutex_t mutexPool;  //锁整个线程池
    pthread_mutex_t mutexBusy;  //锁工作中线程数变量
    pthread_cond_t notFull;     //任务队列满了？
    pthread_cond_t notEmpty;    //任务队列空了?
    int shutdown;               //销毁线程池信号，1销毁，0不销毁
};

typedef struct ThreadPool ThreadPool;

//创建线程池并初始化
ThreadPool *threadPoolCreate(int min, int max, int queueSize);
int threadPoolDestroy(ThreadPool* pool); //销毁
void threadPoolAdd(ThreadPool* pool, void(*func)(void*), void* arg); //添加任务
int threadPoolBusyNum(ThreadPool* pool);  //获得工作中线程个数
int threadPoolAliveNum(ThreadPool* pool); //获得存活线程个数
void* worker(void* arg); //消费者线程
void* manager(void* arg); //管理者线程
void threadExit(ThreadPool* pool); //单线程退出

