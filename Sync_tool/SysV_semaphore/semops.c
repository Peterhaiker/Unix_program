/*
 * semops.c
 * Copyright (C) 2017-10-25 17:13 
 * author  Peterhaiker 
 * email   <vim.memory@gmail.com>
 *
 * description:
 */

#include "stdio.h"
#include<sys/sem.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>

int main(int argc,char*argv[])
{
  errno=0;
  if(argc<2){
    puts("usage:./semops [-n] [-u] <filename> num1 num2...");
    return errno;
  }
  int flags=0;
  int i;
  //analyse argument passed by main function
  if(argv[1][0]=='-'){
    while(-1!=(i=getopt(argc,argv,"nu"))){
      switch(i){
        case 'n':
          flags|=IPC_NOWAIT;break;
        case 'u':
          flags|=SEM_UNDO;break;
      }
      if(argv[optind][0]!='-')
        break;
    }
  }
  //get semaphore id
  int semid;
  semid=semget(ftok(argv[optind],0),0,0);
  optind++;

  //get the number of semaphore we want to modify and allocated the space to store the operation number
  struct sembuf *ptr;
  int num=argc-optind;
  ptr=calloc(num,sizeof(struct sembuf));

  //assignment the ptr array
  for(int i=0;i<num;i++){
    ptr[i].sem_num=i;
    ptr[i].sem_op=atoi(argv[optind+i]);
    ptr[i].sem_flg=flags;
  }
  semop(semid,ptr,num);
  return 0;
}
