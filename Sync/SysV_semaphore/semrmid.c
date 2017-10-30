/*
 * semrmid.c
 * Copyright (C) 2017-10-25 12:51 
 * author  Peterhaiker 
 * email   <vim.memory@gmail.com>
 *
 * description:
 */

#include "stdio.h"
#include<sys/sem.h>
#include<unistd.h>
#include<errno.h>

int main(int argc,char*argv[])
{
  errno=0;
  if(argc!=2){
    puts("usage:./semrmid <filename>");
    return errno;
  }
  int semid;
  semid=semget(ftok(argv[1],0),0,0);
  if(-1==semctl(semid,0,IPC_RMID)){
    puts("remove sysV semaphore failed");
    return errno;
  }
  return errno;
}
