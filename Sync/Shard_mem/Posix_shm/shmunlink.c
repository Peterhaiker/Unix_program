/*
 * shmunlink.c
 * Copyright (C) 2017-10-26 20:27 
 * author  Peterhaiker 
 * email   <vim.memory@gmail.com>
 *
 * description:
 */

#include "stdio.h"
#include<fcntl.h>
#include<unistd.h>
#include<sys/mman.h>
#include<errno.h>

int main(int argc,char*argv[])
{
  errno=0;
  if(argc!=2){
    puts("usage:./shmunlink <filename>");
    return errno;
  }
  shm_unlink(argv[1]);
  return 0;
}
