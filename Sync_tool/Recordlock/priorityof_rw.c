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
    fd=open("a.txt",O_CREAT|O_RDWR,0644);
    if(fd<=0){
        perror("create file failed");
        return errno;
    }
    //now set a read lock to the file
    pid_t pid;
    struct flock lock;
    lock.l_type=F_RDLCK;
    lock.l_whence=SEEK_SET;
    lock.l_start=0;
    lock.l_len=0;
    if(-1==fcntl(fd,F_SETLK,&lock)){
        perror("main process set read lock failed");
        return errno;
    }
    puts("main process set read lock");
    if((pid=fork())<0){
        perror("create child 1 failed");
        return errno;
    }
    else if(0==pid){
        lock.l_type=F_WRLCK;
        lock.l_whence=SEEK_SET;
        lock.l_start=0;
        lock.l_len=0;
        if(-1==fcntl(fd,F_SETLKW,&lock)){
            perror("child 1 set write lock failed");
            return 0;
        }
        puts("child 1 set write lock");
        //ensure father process running
        sleep(1);
        lock.l_type=F_UNLCK;
        if(-1==fcntl(fd,F_SETLKW,&lock)){
            perror("child 1 unlock lock failed");
            return 0;
        }
        return 0;
    }
    //sleep 1 second to ensure therad 1 try to get write lock
    sleep(1);
    if((pid=fork())<0){
        perror("create child 2 failed");
        return errno;
    }
    else if(0==pid){
        lock.l_type=F_RDLCK;
        lock.l_whence=SEEK_SET;
        lock.l_start=0;
        lock.l_len=0;
        if(-1==fcntl(fd,F_SETLKW,&lock)){
            perror("child 2 set read lock failed");
            return 0;
        }
        puts("child 2 set read lock");
        lock.l_type=F_UNLCK;
        if(-1==fcntl(fd,F_SETLKW,&lock)){
            perror("child 2 unlock failed");
            return 0;
        }
        return 0;
    }
    //ensure thread 2 run first
    sleep(1);
    lock.l_type=F_UNLCK;
    if(-1==fcntl(fd,F_SETLKW,&lock)){
        perror("main process unlock read lock failed");
        return errno;
    }
    while(-1!=waitpid(-1,NULL,WNOHANG));
    return 0;
}
