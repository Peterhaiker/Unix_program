/*
 * shmread.c
 * Copyright (C) 2017-10-26 22:38 
 * author  Peterhaiker 
 * email   <vim.memory@gmail.com>
 *
 * description:
 */

#include "stdio.h"
#include<sys/mman.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<errno.h>

int main(int argc,char*argv[])
{
  errno=0;
  if(argc!=2){
    puts("usage:./shmread <filename>");
    return errno;
  }
  int fd=shm_open(argv[1],O_RDONLY,0);
  struct stat stat;
  fstat(fd,&stat);
  char *ptr;
  ptr=mmap(NULL,stat.st_size,PROT_READ,MAP_SHARED,fd,0);

  return 0;
}
