/*
 * door_client.c
 * Copyright (C) 2017-10-30 12:27 
 * author  Peterhaiker 
 * email   <vim.memory@gmail.com>
 *
 * description:
 */

#include "stdio.h"
#include<fcntl.h>
#include<door.h>
#include"mydoor.h"
#include<errno.h>


int main(int argc,char*argv[])
{
  errno=0;
  if(argc!=2){
    puts("usage:./door_client <integer>");
    return errno;
  }
  int fd;
  fd=open(DOOR_PATH,O_RDWR);
  door_arg_t arg;
  int ival,oval;
  ival=atoi(argv[1]);
  arg.date_ptr=&ival;
  arg.date_size=sizeof(int);
  arg.desc_ptr=NULL;
  arg.desc_num=0;
  arg.rbuf=(char*)&oval;
  arg.rsize=sizeof(int);
  door_call(fd,&arg);
  printf("result=%d\n",oval);
  return 0;
}
