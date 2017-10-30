/*
 * shmwrite.c
 * Copyright (C) 2017-10-27 15:34 
 * author  Peterhaiker 
 * email   <vim.memory@gmail.com>
 *
 * description:打开一个文件并扩展它的大小，父进程存储0 1 2...255 0 1...形式的数，然后子进程也以此格式读，若发现错误形式则输出这个形式。父子进程通过POSIX信号量来实现同步
 */

#include "stdio.h"
#include<sys/mman.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<stdlib.h>
#include<semaphore.h>
#include<fcntl.h>
#include<errno.h>

int main(int argc,char*argv[])
{
  errno=0;
  int i;
  int flag=O_CREAT|O_RDWR;
  //解析参数
  while(-1!=(i=getopt(argc,argv,"e"))){
    switch(i){
      case 'e':
        flag|=O_EXCL;break;
    }
  }
  //判断参数是否符合要求
  if(argc!=optind+2){
    puts("usage:./shmwrite [-e] <filename> <size>");
    return errno;
  }
  //打开或创建指定的文件
  //防止文件已存在
  unlink(argv[optind]);
  int fd;
  fd=open(argv[optind],flag,0644);
  optind++;
  int size=atoi(argv[optind]);
  ftruncate(fd,size);

  int*ptr=mmap(NULL,size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
  //在父进程中创建的POSIX信号量可以穿透fork和子进程共享
  sem_t *mutex,*nstored,*nempty;
  //防止内核中存在这些信号量，所以先删除，没有也没关系
  sem_unlink("/sem_mutex");
  sem_unlink("/sem_nstored");
  sem_unlink("/sem_nempty");
  mutex=sem_open("/sem_mutex",O_CREAT,S_IRUSR|S_IWUSR,1);
  nstored=sem_open("/sem_nstored",O_CREAT,S_IRUSR|S_IWUSR,0);
  nempty=sem_open("/sem_nempty",O_CREAT,S_IRUSR|S_IWUSR,size);
  //在fork之后父进程的共享内存区子进程可以看到，这可以作为父子进程通信的办法
  pid_t pid;
  if((pid=fork())<0){
    puts("fork failed");
    munmap(ptr,size);
    return errno;
  }
  else if(0==pid){
    //child write date to shared memory
    int *p=ptr;
    for(int i=0;i<size;++i){
      sem_wait(nempty);
      sem_wait(mutex);
      *p++=i%256;
      sem_post(mutex);
      sem_post(nstored);
    }
    return 0;
  }
  //father read the date in shared memory
  int *p=ptr;
  for(int i=0;i<size;++i){
    sem_wait(nstored);
    sem_wait(mutex);
    if(*p!=i%256)
      printf("p[%d]=%d\n",i,p[i]);
    p++;
    msync(p,size,MS_SYNC);
    sem_post(mutex);
    sem_post(nempty);
  }
  puts("success");
  unlink(argv[optind-1]);
  munmap(ptr,size);
  wait(NULL);
  return 0;
}
