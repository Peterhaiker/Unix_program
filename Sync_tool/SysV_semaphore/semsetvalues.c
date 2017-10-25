/*
 * semsetvalues.c
 * Copyright (C) 2017-10-25 16:14 
 * author  Peterhaiker 
 * email   <vim.memory@gmail.com>
 *
 * description:
 */

#include "stdio.h"
#include<stdlib.h>
#include<sys/sem.h>
#include<errno.h>

int main(int argc,char*argv[])
{
  errno=0;
  if(argc<2){
    puts("usage:./semsetvalues <filename> [value1,value2...]");
    return errno;
  }
  //get semaphore id
  int semid;
  semid=semget(ftok(argv[1],0),0,0);

  int nsems;
  struct semid_ds buf;
  union semun{
    int val;
    struct semid_ds*buf;
    unsigned short*array;
  }arg;
  //get the number of semaphore in a semaphore set
  arg.buf=&buf;
  semctl(semid,0,IPC_STAT,arg);
  nsems=arg.buf->sem_nsems;
  //define a array to store value passed by parameter
  unsigned short val[nsems];
  for(int i=2;i<argc;++i)
    val[i-2]=atoi(argv[i]);
  arg.array=val;
  semctl(semid,0,SETALL,arg);
  return 0;
}
