//create or open a semaphore file
#include<semaphore.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>

int main(int argc,char*argv[])
{
  errno=0;
  if(argc<2){
    puts("usage:./semcreate [-e] [-i value] <name>");
    return errno;
  }
  int flags;
  flags=O_CREAT|O_RDWR;  //default flags if not specified e option
  int value=1;  //default semaphore value if not specified i option
  int c;
  while((c=getopt(argc,argv,"ei:"))!=-1){
    switch(c){
      case 'e':
        flags|=O_EXCL;break;
      case 'i':
        value=atoi(optarg);
        break;
    }
  }
  if(optind!=argc-1){
    puts("usage:./semcreate [-e] [-i value] <name>");
    return errno;
  }
  sem_t*sem;
  if(SEM_FAILED==(sem=sem_open(argv[optind],flags,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH,value))){
    puts("create semaphore failed");
    return errno;
  }
  sem_close(sem);
  return errno;
}
