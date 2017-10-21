/*
 * semgetvalue.c
 * Copyright (C) 2017-10-21 17:36 
 * author  Peterhaiker 
 * email   <vim.memory@gmail.com>
 *
 * description:
 */

#include "stdio.h"
#include<semaphore.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<errno.h>

int main(int argc,char*argv[])
{
  errno=0;
  if(argc!=2){
    puts("usage:./semgetvalue <name>");
    return errno;
  }
  sem_t*sem;
  int val;
  if((sem=sem_open(argv[1],0))!=SEM_FAILED){
    if(-1!=sem_getvalue(sem,&val)){
      printf("semaphore value:%d\n",val);
      return errno;
    }
  }
  puts("get semaphore failed");
  return errno;
}
