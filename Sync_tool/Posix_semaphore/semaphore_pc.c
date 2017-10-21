/*
 * semaphore_pc.c
 * Copyright (C) 2017-10-21 22:18 
 * author  Peterhaiker 
 * email   <vim.memory@gmail.com>
 *
 * description:
 */
//一开始仿照互斥量和条件变量写出的生产者消费者，发现无法等待条件唤醒，于是参考书上发现只要使用多个信号量来取代互斥量和条件变量的组合也可以
#include "stdio.h"
#include<semaphore.h>
#include<pthread.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/stat.h>
#include<errno.h>

struct{
  int buf[10];
  sem_t*mutex,*nempty,*nstored;
}shared;

void*consumer(void*arg)
{
  for(int i=0;i<1000;i++){
    sem_wait(shared.nempty);
    sem_wait(shared.mutex);
    shared.buf[i%10]=i;
    sem_post(shared.mutex);
    sem_post(shared.nstored);
  }
  return 0;
}

void*producer(void*arg)
{
  for(int i=0;i<1000;i++){
    sem_wait(shared.nstored);
    sem_wait(shared.mutex);
    if(shared.buf[i%10]!=i)
      printf("buf[%d]=%d\n",i%10,i);
    sem_post(shared.mutex);
    sem_post(shared.nempty);
  }
}

int main(int argc,char*argv[])
{
  shared.mutex=sem_open("mutex",O_CREAT|O_RDWR,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH,1);
  shared.nempty=sem_open("nempty",O_CREAT|O_RDWR,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH,10);
  shared.nstored=sem_open("nstored",O_CREAT|O_RDWR,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH,0);
  pthread_t consumer_id,producer_id;
  pthread_create(&consumer_id,NULL,consumer,NULL);
  pthread_create(&producer_id,NULL,producer,NULL);

  pthread_join(consumer_id,NULL);
  pthread_join(producer_id,NULL);
  sem_unlink("mutex");
  sem_unlink("nempty");
  sem_unlink("nstored");
  puts("success");
  return 0;
}
