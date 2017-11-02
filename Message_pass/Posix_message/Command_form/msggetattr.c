/*
 * msggetattr.c
 * Copyright (C) 2017-11-02 20:49 
 * author  Peterhaiker 
 * email   <vim.memory@gmail.com>
 *
 * description:
 */

#include "stdio.h"
#include<string.h>
#include<mqueue.h>
#include<unistd.h>
#include<errno.h>

int main(int argc,char*argv[])
{
  errno=0;
  if(argc!=2){
    puts("usage:./msggetattr filename");
    return errno;
  }
  struct mq_attr attr;
  mqd_t mq;
  char filename[50]={'\0'};
  char*p=strrchr(argv[1],'/');
  if(p==NULL){
    filename[0]='/';
    p=argv[1];
  }
  strncat(filename,p,50);
  if((mq=mq_open(filename,O_RDONLY))==-1){
    puts("open message queue failed");
    return errno;
  }
  mq_getattr(mq,&attr);
  printf("mq_flags:%s\n",attr.mq_flags&O_NONBLOCK?"nonblock":"block");
  printf("max number of messages allowed on queue:%ld\n",attr.mq_maxmsg);
  printf("max size of a message (in bytes):%ld\n",attr.mq_msgsize);
  printf("number of messages currently on queue:%ld\n",attr.mq_curmsgs);
  mq_close(mq);
  return 0;
}
