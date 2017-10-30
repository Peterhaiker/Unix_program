/*
 * semtrywait.c
 * Copyright (C) 2017-10-21 18:28 
 * author  Peterhaiker 
 * email   <vim.memory@gmail.com>
 *
 * description:
 */

#include "stdio.h"
#include<semaphore.h>
#include<unistd.h>
#include<errno.h>

int main(int argc,char*argv[])
{
  errno=0;
  if(argc!=2){
    puts("usage:./semtrywait <filename>");
    return errno;
  }
  sem_t*sem;
  if(SEM_FAILED!=(sem=sem_open(argv[1],0))){
    if(0==sem_trywait(sem)){
      int value=-1;
      sem_getvalue(sem,&value);
      printf("pid:%d,semaphore value:%d\n",getpid(),value);
      return errno;
    }
  }
  puts("wait failed");
  return errno;
}
