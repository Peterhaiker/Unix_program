/*---------------------------------------------
 *priorityof_rw.c
 *Copyright (C) 2017-10-16 23:28 
 *author Peterhaiker 
 *mail   <vim.memory@gmail.com>
 *----------------------------------------------
 *Description:
 */

#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<fcntl.h>
#include<errno.h>

int fd;
void*th1(void*arg){
    struct flock lock;
    lock.l_type=F_WRLCK;
    lock.l_whence=SEEK_SET;
    lock.l_start=0;
    lock.l_len=0;
    if(-1==fcntl(fd,F_SETLKW,&lock)){
        perror("thread 1 set write lock failed");
        return 0;
    }
    puts("thread 1 set write lock");
    lock.l_type=F_UNLCK;
    if(-1==fcntl(fd,F_SETLKW,&lock)){
        perror("thread 1 unlock lock failed");
        return 0;
    }
}

void*th2(void*arg){
    struct flock lock;
    lock.l_type=F_RDLCK;
    lock.l_whence=SEEK_SET;
    lock.l_start=0;
    lock.l_len=0;
    if(-1==fcntl(fd,F_SETLKW,&lock)){
        perror("thread 2 set read lock failed");
        return 0;
    }
    puts("thread 2 set read lock");
    lock.l_type=F_UNLCK;
    if(-1==fcntl(fd,F_SETLKW,&lock)){
        perror("thread 2 unlock failed");
        return 0;
    }
}

int main(int argc,char*argv[])
{
    errno=0;
    fd=open("a.txt",O_CREAT|O_RDWR,0644);
    if(fd<=0){
        perror("create file failed");
        return errno;
    }
    //now set a read lock to the file
    pthread_t tid1,tid2;
    struct flock lock;
    lock.l_type=F_RDLCK;
    lock.l_whence=SEEK_SET;
    lock.l_start=0;
    lock.l_len=0;
    if(-1==fcntl(fd,F_SETLK,&lock)){
        perror("main thread set read lock failed");
        return errno;
    }
    puts("main process set read lock");
    if(-1==pthread_create(&tid1,NULL,th1,NULL)){
        perror("create thread 1 failed");
        return errno;
    }
    //sleep 1 second to ensure therad 1 try to get write lock
    sleep(1);
    if(-1==pthread_create(&tid2,NULL,th2,NULL)){
        perror("create thread 2 failed");
        return errno;
    }
    //ensure thread 2 run first
    sleep(1);
    lock.l_type=F_UNLCK;
    if(-1==fcntl(fd,F_SETLKW,&lock)){
        perror("main thread unlock read lock failed");
        return errno;
    }
    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);
    return 0;
}
