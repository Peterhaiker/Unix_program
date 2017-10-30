/*
 * sempost.c
 * Copyright (C) 2017-10-21 18:41 
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
    puts("usage:./sempost <filename>");
    return errno;
  }
  sem_t *sem;
  if(SEM_FAILED!=(sem=sem_open(argv[1],0))){
    if(0==sem_post(sem)){
      int value;
      sem_getvalue(sem,&value);
      printf("pid:%d,semaphore value:%d\n",getpid(),value);
      return errno;
    }
  }
  puts("post semaphore failed");
  return 0;
}
