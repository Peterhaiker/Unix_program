/*
 * shmcreate.c
 * Copyright (C) 2017-10-26 20:02 
 * author  Peterhaiker 
 * email   <vim.memory@gmail.com>
 *
 * description:
 */

#include "stdio.h"
#include<sys/mman.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdlib.h>
#include<errno.h>

int main(int argc,char*argv[])
{
  errno=0;
  int i;
  int flag=O_CREAT|O_RDWR;
  while(-1!=(i=getopt(argc,argv,"e"))){
    switch(i){
      case 'e':
        flag|=O_EXCL;break;
    }
  }
  if(optind!=argc-2){
    puts("usage:./shmcreate [-e] <name> <length>");
    return errno;
  }
  int length=atoi(argv[optind+1]);
  int fd;
  fd=shm_open(argv[optind],flag,0644);
  ftruncate(fd,length);
  
  char*ptr;
  ptr=mmap(NULL,length,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
  return errno;
}
