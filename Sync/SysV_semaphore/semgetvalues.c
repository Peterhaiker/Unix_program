/*
 * semgetvalues.c
 * Copyright (C) 2017-10-25 15:35 
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
  if(argc!=2){
    puts("usage:./semgetvalues <filename>");
    return errno;
  }
  //get semaphore id
  int semid;
  semid=semget(ftok(argv[1],0),0,0);
  //get above semaphore id specified semid_ds structure
  unsigned short *ptr;
  struct semid_ds buf;
  union semun{
    int val;
    struct semid_ds*buf;
    unsigned short *array;
  }arg;
  arg.buf=&buf;
  semctl(semid,0,IPC_STAT,arg);
  //get the semaphore number in the semaphore set
  int nsems;
  nsems=arg.buf->sem_nsems;
  //allocate enough space to ptr to store each semaphore value
  ptr=calloc(nsems,sizeof(unsigned short));
  arg.array=ptr;
  //fetch the value and print
  semctl(semid,0,GETALL,arg);
  for(int i=0;i<nsems;++i)
    printf("semval[%d]=%d\n",i,arg.array[i]);
  return 0;
}
