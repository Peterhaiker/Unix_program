/*
 * anonymous_semaphore_pc.c
 * Copyright (C) 2017-10-22 16:44 
 * author  Peterhaiker 
 * email   <vim.memory@gmail.com>
 *
 * description:anonymous posix semaphore to realize multi consumer multi producer
 */

#include "stdio.h"
#include<pthread.h>
#include<semaphore.h>
#include<errno.h>

#define MAX 10000
struct{
  int buf[10];
  int nput,putval;
  int nget,getval;
  sem_t mutex,nempty,nstored;
  int product_count,consume_count;
}shared;

void*consumer(void*arg)
{
  for(;;){
    sem_wait(&shared.nstored);
    sem_wait(&shared.mutex);
    if(shared.consume_count>=MAX){//如果生产了足够的数量则退出
      sem_post(&shared.mutex);
      sem_post(&shared.nstored);
      pthread_exit(0);
    }
    if(shared.buf[shared.nget%10]!=shared.getval)
      printf("consumer_id[%lu]:buf[%d]=%d\n",pthread_self(),shared.nget%10,shared.buf[shared.nget%10]);
    shared.nget++;
    shared.getval++;
    shared.consume_count++;
    *((int*)arg)+=1;
    sem_post(&shared.mutex);
    sem_post(&shared.nempty);
  }
}

void*producer(void*arg)
{
  for(;;){
    sem_wait(&shared.nempty);
    sem_wait(&shared.mutex);
    if(shared.product_count>=MAX){
      sem_post(&shared.mutex);
      sem_post(&shared.nempty);
      sem_post(&shared.nstored);//防止消费者阻塞在sem_wait(&shared.nempty)条件上
      pthread_exit(0);
    }
    shared.buf[shared.nput%10]=shared.putval;
    shared.nput++;
    shared.putval++;
    shared.product_count++;
    *((int*)arg)+=1;
    sem_post(&shared.mutex);
    sem_post(&shared.nstored);
  }
}
int main(int argc,char*argv[])
{
  sem_init(&shared.mutex,0,1);
  sem_init(&shared.nempty,0,10);
  sem_init(&shared.nstored,0,0);
  shared.product_count=shared.consume_count=0;
  shared.nput=shared.nget=shared.getval=shared.putval=0;
  pthread_t producer_id[10],consumer_id[10];
  int nproduct[10],nconsumer[10];
  int i;
  pthread_setconcurrency(0);
  for(i=0;i<10;++i){
    nproduct[i]=0;
    pthread_create(&producer_id[i],NULL,producer,&nproduct[i]);
  }
  for(i=0;i<10;++i){
    nconsumer[i]=0;
    pthread_create(&consumer_id[i],NULL,consumer,&nconsumer[i]);
  }

  for(i=0;i<10;i++)
    pthread_join(producer_id[i],NULL);
  for(i=0;i<10;++i)
    pthread_join(consumer_id[i],NULL);

  for(i=0;i<10;++i)
    printf("producer[%d]=%d\n",i+1,nproduct[i]);
  for(i=0;i<10;++i)
    printf("consumer[%d]=%d\n",i+1,nconsumer[i]);
  return 0;
}
