/*
 * semunlink.c
 * Copyright (C) 2017-10-21 17:32 
 * author  Peterhaiker 
 * email   <vim.memory@gmail.com>
 *
 * description:
 */

#include<semaphore.h>
#include<errno.h>
#include "stdio.h"

int main(int argc,char*argv[])
{
  errno=0;
  if(argc!=2){
    puts("usage:./semunlink <name>");
    return errno;
  }
  sem_unlink(argv[1]);
  return 0;
}
