/*
 * shmwrite.c
 * Copyright (C) 2017-10-26 22:29 
 * author  Peterhaiker 
 * email   <vim.memory@gmail.com>
 *
 * description:
 */

#include "stdio.h"
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/mman.h>
#include<errno.h>

int main(int argc,char*argv[])
{
  errno=0;
  if(argc!=2){
    puts("usage:./shmwrite <filename>");
    return errno;
  }
  int fd=shm_open(argv[1],O_RDWR,0644);
  struct stat stat;
  fstat(fd,&stat);
  char *ptr;
  ptr=mmap(NULL,stat.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
  close(fd);

  for(int i=0;i<stat.st_size;++i)
    *ptr++=i%256;
  return 0;
}
