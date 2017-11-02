/*
 * msgcreate.c
 * Copyright (C) 2017-11-02 18:07 
 * author  Peterhaiker 
 * email   <vim.memory@gmail.com>
 *
 * description:create a posix message queue but not open
 */

#include "stdio.h"
#include<string.h>
#include<mqueue.h>
#include<unistd.h>
#include<errno.h>

int main(int argc,char*argv[])
{
  errno=0;
  int n;
  int flag=O_CREAT|O_RDWR;
  while((n=getopt(argc,argv,"ne"))!=-1){
    switch(n){
      case 'n':
        flag|=O_NONBLOCK;break;
      case 'e':
        flag|=O_EXCL;break;
    }
  }
  if(argc!=optind+1){
    puts("usage:./msgcreate [-e|-n] <filename>");
    return errno;
  }
  //add a '/' in the begining of the filename
  char*p=strrchr(argv[optind],'/');
  char filename[50]={'\0'};
  if(p==NULL){
    filename[0]='/';
    p=argv[optind];
  }
  strncat(filename,p,sizeof(filename));

  //test
  printf("filename:%s\n",filename);
  mqd_t mq=mq_open(filename,flag,S_IRUSR|S_IWUSR,NULL);
  if(-1==mq){
    puts("create message queue failed");
    return errno;
  }
  mq_close(mq);
  return 0;
}
