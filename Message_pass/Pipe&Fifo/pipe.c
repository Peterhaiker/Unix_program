/*
 * fifo.c
 * Copyright (C) 2017-10-30 20:18 
 * author  Peterhaiker 
 * email   <vim.memory@gmail.com>
 *
 * description:father pass a filename to child,then child print its content
 * NOTICE:the pipe buffer size
 */

#include "stdio.h"
#include<sys/wait.h>
#include<string.h>
#include<limits.h>
#include<unistd.h>
#include<pthread.h>
#include<fcntl.h>
#include<errno.h>

int main(int argc,char*argv[])
{
  errno=0;
  int fd[2];
  pipe(fd);
  int pid;
  int filenamesize;
  //get filename max size
  filenamesize=pathconf("fifo.c",_PC_NAME_MAX);
  filenamesize=filenamesize>100?100:filenamesize;
  char filename[filenamesize];//this is a VLA变长数组,so can't initialize

  if((pid=fork())<0){
    puts("create child process failed");
    return errno;
  }
  else if(pid>0){
    close(fd[0]);
    printf("input filename(^R to quit):");
    if(NULL==fgets(filename,filenamesize,stdin)){
      puts("read filename failed");
      return errno;
    }

    if(10==(int)filename[0]){ //input ^R character
      puts("bye");
      close(fd[1]);
      wait(NULL);
      return errno;
    }
    //clear character \n
    filename[strlen(filename)-1]='\0';
    write(fd[1],filename,filenamesize);
  }
  //child process
  close(fd[1]);
  int filed;
  char buffer[1024]={'\0'};
  int n;//store the number of character read
  if(-1==read(fd[0],filename,filenamesize)){
    puts("read from pipe failed");
    return errno;
  }
  //read nothing
  if(0==strlen(filename)){
    puts("nothing in buffer");
    return errno;
  }
  filed=open(filename,O_RDONLY);
  //open file failed
  if(filed<0){
    printf("can't open %s\n",filename);
    return errno;
  }
  //read file content
  while(0<(n=read(filed,buffer,1024))){
    write(STDOUT_FILENO,buffer,n);
  }
  if(n<0)
    puts("read file failed");
  return errno;
}
