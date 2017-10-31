/*
 * pipe.c
 * Copyright (C) 2017-10-30 22:41 
 * author  Peterhaiker 
 * email   <vim.memory@gmail.com>
 *
 * description:
 */

#include "stdio.h"
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<errno.h>

int main(int argc,char*argv[])
{
  errno=0;
  int fd[2];
  int i=pipe(fd);
  if(i<0){
    puts("pipe failed");
    return errno;
  }
  int a=2;
  if(fork()>0){
    close(fd[0]);
    write(fd[1],&a,sizeof(int));
    int err=-1;
    wait(&err);
    printf("father:a=%d\n",a);
    return err;
  }
  close(fd[1]);
  a=-1;
  int n=read(fd[0],&a,sizeof(int));
  printf("a=%d,n=%d\n",a,n);
  a=-1;
  n=read(fd[0],&a,sizeof(int));
  printf("second:a=%d,n=%d\n",a,n);
  return errno;
}
