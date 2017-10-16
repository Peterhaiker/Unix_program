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
#include<sys/wait.h>
#include<sys/types.h>
#include<fcntl.h>
#include<errno.h>

int main(int argc,char*argv[])
{
    errno=0;
    int fd;
    pid_t pid1,pid2;
    fd=open("a.txt",O_CREAT|O_RDWR,0644);
    if(fd<=0){
        perror("create file failed");
        return errno;
    }
    //now set a read lock to the file
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
    if((pid1=fork())<0){
        perror("create process 1 failed");
        return errno;
    }
    else if(pid1==0){
        errno=0;
        lock.l_type=F_WRLCK;
        lock.l_whence=SEEK_SET;
        lock.l_start=0;
        lock.l_len=0;
        if(-1==fcntl(fd,F_SETLKW,&lock)){
            perror("child 1 set write lock failed");
            return errno;
        }
        puts("child 1 set write lock");
        lock.l_type=F_UNLCK;
        if(-1==fcntl(fd,F_SETLKW,&lock)){
            perror("child 1 unlock write lock failed");
            return errno;
        }
        return 0;
    }
    //sleep 1 second to ensure child 1 try to get write lock
    sleep(1);
    if((pid2=fork())<0){
        perror("create child 2 failed");
        return errno;
    }
    else if(pid2==0){
        errno=0;
        lock.l_type=F_RDLCK;
        lock.l_whence=SEEK_SET;
        lock.l_start=0;
        lock.l_len=0;
        if(-1==fcntl(fd,F_SETLKW,&lock)){
            perror("child 2 set read lock failed");
            return errno;
        }
        puts("child 2 set read lock");
        lock.l_type=F_UNLCK;
        if(-1==fcntl(fd,F_SETLKW,&lock)){
            perror("child 2 unlock read lock failed");
            return errno;
        }
        return 0;
    }
    //ensure child 2 run first
    sleep(1);
    lock.l_type=F_UNLCK;
    if(-1==fcntl(fd,F_SETLKW,&lock)){
        perror("main thread unlock read lock failed");
        return errno;
    }
    while(-1!=waitpid(-1,NULL,WNOHANG))
        ;
    return 0;
}

