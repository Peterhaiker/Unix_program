/*
 * anonymous_multi_buffer_semaphore_pc.c
 * Copyright (C) 2017-10-22 22:07 
 * author  Peterhaiker 
 * email   <vim.memory@gmail.com>
 *
 * description:a producer,a consumer.multi buffer to support to consumer and producer
 */

#include "stdio.h"
#include<stdlib.h>
#include<semaphore.h>
#include<errno.h>
#include<pthread.h>

#define NBUFF 8
#define MAX 10000
struct{
  struct{
    int data[10];
    ssize_t n;
  }buff[NBUFF]; //8 buffer support
  int count;    //if count add to 10000,then quit all producer
  int consume_index,product_index;
  sem_t mutex,nempty,nstored;
}shared;

void*producer(void*arg)
{
  for(;;){
    sem_wait(&shared.nempty);
    sem_wait(&shared.mutex);
    //if product MAX numbers,then quit
    if(shared.count>=MAX){
      sem_post(&shared.nstored);
      sem_post(&shared.mutex);
      sem_post(&shared.nempty);
      pthread_exit(0);
    }

    //write 10 integer to buffer
    for(int i=0;i<10;++i){
      shared.buff[shared.product_index].data[i]=rand()%10000;
      shared.buff[shared.product_index].n++;
      shared.count++;
      *((int*)arg)+=1;
    }

    shared.product_index++;
    shared.product_index%=NBUFF;
    sem_post(&shared.mutex);
    sem_post(&shared.nstored);
  }
}

void*consumer(void*arg)
{
  for(;;){
    sem_wait(&shared.nstored);
    sem_wait(&shared.mutex);

    if(shared.count>=MAX){
      sem_post(&shared.mutex);
      sem_post(&shared.nstored);
      pthread_exit(0);
    }

    for(int i=0;i<shared.buff[shared.consume_index].n;i++){
      if(-1==shared.buff[shared.consume_index].data[i])
        printf("shared.buff[%d].data[%d]=-1\n",shared.consume_index,i);
      shared.buff[shared.consume_index].data[i]=-1;
      *((int*)arg)+=1;
    }
    shared.buff[shared.consume_index].n=0;
    shared.consume_index++;
    shared.consume_index%=NBUFF;
    sem_post(&shared.mutex);
    sem_post(&shared.nempty);
  }
}

int main(int argc,char*argv[])
{
  errno=0;
  sem_init(&shared.mutex,0,1);
  sem_init(&shared.nempty,0,NBUFF);
  sem_init(&shared.nstored,0,0);
  pthread_t consumer_id[8],producer_id[8];
  int nconsumer[8],nproducer[8];

  int i;
  for(i=0;i<8;++i){
    nconsumer[i]=0;
    pthread_create(&producer_id[i],NULL,producer,&nproducer[i]);
  }
  for(i=0;i<8;++i){
    nproducer[i]=0;
    pthread_create(&consumer_id[i],NULL,consumer,&nconsumer[i]);
  }

  for(i=0;i<8;++i)
    pthread_join(producer_id[i],NULL);
  for(i=0;i<8;++i)
    pthread_join(consumer_id[i],NULL);

  for(i=0;i<8;++i)
    printf("producer[%d]=%d\n",i+1,nproducer[i]);
  for(i=0;i<8;++i)
    printf("consumer[%d]=%d\n",i+1,nconsumer[i]);

  sem_destroy(&shared.mutex);
  sem_destroy(&shared.nempty);
  sem_destroy(&shared.nstored);
  return 0;
}
