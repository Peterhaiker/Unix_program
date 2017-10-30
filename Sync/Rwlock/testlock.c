/*---------------------------------------------
 * testmylock.c
 * Copyright (C) 2017-10-16 00:06 
 *author Peterhaiker 
 *mail   <vim.memory@gmail.com>
 *----------------------------------------------
 *Description:
 */

#include<stdio.h>
#include<sys/time.h>
#include<pthread.h>
#include<unistd.h>
//#include"myrwlock.h"

pthread_rwlock_t rw=PTHREAD_RWLOCK_INITIALIZER;
pthread_t tid1,tid2,tid3;

void*th1(void*arg)
{
    pthread_rwlock_rdlock(&rw);
    puts("th1:get read lock");
    sleep(3);
    pthread_rwlock_unlock(&rw);
    pthread_exit(NULL);
}

void*th2(void*arg)
{
    pthread_rwlock_wrlock(&rw);
    puts("th2:get write lock");
    pthread_rwlock_unlock(&rw);
    pthread_exit(NULL);
}

void*th3(void*arg)
{
    pthread_rwlock_rdlock(&rw);
    puts("th3:get read lock");
    pthread_rwlock_unlock(&rw);
    pthread_exit(NULL);
}
int main(int argc,char*argv[])
{
    struct timespec start,end,diff;
    clock_gettime(CLOCK_REALTIME,&start);
    if(pthread_create(&tid1,NULL,th1,NULL)!=0){
        perror("create thread 1 failed");
        return 0;
    }
    sleep(1);
    if(pthread_create(&tid2,NULL,th2,NULL)!=0){
        perror("create thread 2 failed");
        return 0;
    }
    if(pthread_create(&tid3,NULL,th3,NULL)!=0){
        perror("create thread 3 failed");
        return 0;
    }
    pthread_join(tid2,NULL);
    clock_gettime(CLOCK_REALTIME,&end);
    diff.tv_sec=end.tv_sec-start.tv_sec;
    if(end.tv_nsec<start.tv_nsec){
        diff.tv_sec-=1;
        diff.tv_nsec=1000000000+end.tv_nsec-start.tv_nsec;
    }
    else
        diff.tv_nsec=end.tv_nsec-start.tv_nsec;
    printf("sys lock spend time:%lds,%ldn\n",diff.tv_sec,diff.tv_nsec);
    return 0;
}

