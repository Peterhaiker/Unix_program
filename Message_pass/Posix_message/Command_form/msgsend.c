/*
 * msgsend.c
 * Copyright (C) 2017-11-02 18:29 
 * author  Peterhaiker 
 * email   <vim.memory@gmail.com>
 *
 * description:
 */

#include "stdio.h"
#include<string.h>
#include<stdlib.h>
#include<mqueue.h>
#include<unistd.h>
#include<errno.h>

int main(int argc,char*argv[])
{
  errno=0;
  int n;
  int flag=O_RDWR;
  int priority=0;
  while((n=getopt(argc,argv,"p:"))!=-1){
    switch(n){
      case 'p':
        priority=atoi(optarg);break;
        
    }
  }
  if(argc!=optind+1){
    puts("usage:./msgsend [-p num] filename");
    return errno;
  }
  //open message queue
  char*p=strrchr(argv[optind],'/');
  char filename[50]={'\0'};
  if(NULL==p){
    filename[0]='/';
    p=argv[optind];
  }
  strncat(filename,p,50);
  mqd_t mq=mq_open(filename,O_WRONLY);
  if(-1==mq){
    puts("open message failed");
    return errno;
  }

  char*buf;
  struct mq_attr attr;
  mq_getattr(mq,&attr);
  buf=calloc(attr.mq_msgsize,sizeof(char));
  printf("input you want to send:");
  fgets(buf,attr.mq_msgsize,stdin);
  mq_send(mq,buf,attr.mq_msgsize,priority);

  free(buf);
  mq_close(mq);
  return 0;
}
