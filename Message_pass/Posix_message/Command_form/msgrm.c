/*
 * msgrm.c
 * Copyright (C) 2017-11-02 18:25 
 * author  Peterhaiker 
 * email   <vim.memory@gmail.com>
 *
 * description:
 */

#include "stdio.h"
#include<string.h>
#include<unistd.h>
#include<mqueue.h>
#include<errno.h>

int main(int argc,char*argv[])
{
  errno=0;
  if(argc!=2){
    puts("usage:./msgrm filename");
    return errno;
  }
  char*p=strrchr(argv[1],'/');
  char filename[50]={'\0'};
  if(NULL==p){
    filename[0]='/';
    p=argv[1];
  }
  strncat(filename,p,50);
  mq_unlink(filename);
  return 0;
}
