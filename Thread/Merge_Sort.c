#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

//归并排序
void Merge(int arr[], int start, int mid, int end);
void* Merge_Sort(void* arg);

int len;
int *ptr; //全局数组指针

int main() {
    printf("Please enter the length of the array:\n");
    scanf("%d", &len);
    
    ptr = (int*)malloc(len * sizeof(int)); //动态分配数组
    if (ptr == NULL) {
        fprintf(stderr, "malloc error\n");
        exit(EXIT_FAILURE);
    }

    printf("Randomly generate random %d numbers in the array:\n", len);
    srand((unsigned)time(NULL));
    for (int i = 0; i < len; i++) {
        ptr[i] = 1 + rand() % 10000;
    }
    for (int i = 0; i < len; i++) {
        printf("%d\t", ptr[i]);
    }
    printf("\n");

    //分配并初始化排序参数
    int arg[2] = {0};
    arg[0] = 0;
    arg[1] = len - 1;

    //创建排序线程
    pthread_t thread;
    pthread_create(&thread, NULL, Merge_Sort, arg);
    pthread_join(thread, NULL);

    printf("The sorted array is:\n");
    for (int i = 0; i < len; i++) {
        printf("%4d\n", ptr[i]);
    }

    free(ptr);
    pthread_exit(0);

    return 0;
}

//递归地对数组进行归并排序
void* Merge_Sort(void* arg) {
    int *argu = (int*)arg;
    int start = argu[0];
    int end = argu[1];

    if (start < end) {
        int mid = (start + end) / 2;

        int arg1[2] = {0};
        int arg2[2] = {0};

        arg1[0] = start;
        arg1[1] = mid;
        arg2[0] = mid + 1;
        arg2[1] = end;

        //创建两个子线程进行递归排序
        pthread_t thread1, thread2;
        pthread_create(&thread1, NULL, Merge_Sort, arg1);
        pthread_create(&thread2, NULL, Merge_Sort, arg2);

        //等待两个子线程完成
        pthread_join(thread1, NULL);
        pthread_join(thread2, NULL);
        
        //合并两个已排序好的子数组
        Merge(ptr, start, mid, end);
    }

    pthread_exit(0);
}

//合并
void Merge(int arr[], int start, int mid, int end) {
    int *tmp = (int*)malloc(len * sizeof(int));
    if (tmp == NULL) {
        fprintf(stderr, "malloc error\n");
        return;
    }

    int i = start, j = mid + 1, k = start;
    //合并子数组
    while (i <= mid && j <= end) {
        if (arr[i] < arr[j]) {
            tmp[k] = arr[i];
            i++;
            k++;
        } else {
            tmp[k] = arr[j];
            k++;
            i++;
        }
    }
    while (i <= mid) {
        tmp[k] = arr[i];
        k++;
        i++;
    }
    while (j <= end) {
        tmp[k] = arr[j];
        k++;
        j++;
    }
    for (int k = start; k <= end; k++) {
        arr[k] = tmp[k];
    }

    free(tmp);
}
