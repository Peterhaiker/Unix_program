/*
 * source_limit.c
 * Copyright (C) 2017-11-02 17:50 
 * author  Peterhaiker 
 * email   <vim.memory@gmail.com>
 *
 * description:show the limits of system to fifo
 */

#include "stdio.h"
#include<unistd.h>
#include<errno.h>

int main(int argc,char*argv[])
{
  errno=0;
  if(argc!=2){
    puts("usage:./source_limit filename");
    return errno;
  }
  printf("PIPE_BUF=%ld,OPEN_MAX=%ld\n",pathconf(argv[1],_PC_PIPE_BUF),sysconf(_SC_OPEN_MAX));
  return 0;
}
