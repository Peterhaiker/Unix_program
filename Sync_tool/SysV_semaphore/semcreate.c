/*
 * semcreate.c
 * Copyright (C) 2017-10-25 12:31 
 * author  Peterhaiker 
 * email   <vim.memory@gmail.com>
 *
 * description:
 */

#include "stdio.h"
#include<sys/sem.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>

int main(int argc,char*argv[])
{
  errno=0;
  if(argc<2){
    puts("usage:./semcreate [-e] <filename> <num>");
    return errno;
  }
  int i;
  int flag=IPC_CREAT|0644;
  while((i=getopt(argc,argv,"e"))!=-1){
    switch(i){
      case 'e':
        flag|=IPC_EXCL;break;
    }
  }
  int semid;
  int num=atoi(argv[optind+1]);
  if(num<=0){
    puts("num of you input must great than 0");
    return errno;
  }
  if(-1==(semid=semget(ftok(argv[optind],0),num,flag))){
    if(EEXIST==errno){
      if(-1==(semid=semget(ftok(argv[optind],0),0,0))){
        puts("file exist,buf open failed");
        return errno;
      }
    }
    else{
    puts("create sysV semaphore failed");
    return errno;
    }
  }
  return 0;
}
