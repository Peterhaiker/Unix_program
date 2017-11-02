/*
 * msgrcv.c
 * Copyright (C) 2017-11-02 20:39 
 * author  Peterhaiker 
 * email   <vim.memory@gmail.com>
 *
 * description:receive message from posix message queue
 */

#include "stdio.h"
#include<string.h>
#include<mqueue.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>

int main(int argc,char*argv[])
{
  errno=0;
  if(argc!=2){
    puts("usage:./msgrcv filename");
    return errno;
  }
  char*p=strrchr(argv[1],'/');
  char filename[50]={'\0'};
  if(p==NULL){
    filename[0]='/';
    p=argv[1];
  }
  strncat(filename,p,50);
  mqd_t mq;
  if((mq=mq_open(filename,O_RDONLY))==-1){
    puts("open message queue failed");
    return errno;
  }
  struct mq_attr attr;
  mq_getattr(mq,&attr);
  char*buf=calloc(attr.mq_msgsize,sizeof(char));
  int priority;
  mq_receive(mq,buf,attr.mq_msgsize,&priority);
  printf("message:%s\npriority:%d\n",buf,priority);
  mq_close(mq);
  return 0;
}
