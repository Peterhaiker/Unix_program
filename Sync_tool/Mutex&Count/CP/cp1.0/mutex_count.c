//one consumer,multi producer,sync by mutex and count
//

#include<pthread.h>
#include<stdio.h>
#include<unistd.h>
#include<errno.h>

#define MAX 1024

int total=0;
struct product{
    int producer_index;   //the next position producer will produce
    int consumer_index;   //the next position consumer will consume
    int free_space;       //unuse space
    int used_space;       //used space
    int buf[MAX];         //the buffer that store product
    pthread_mutex_t mutex;
    pthread_cond_t product_cond,consumer_cond;
}product_queue={0,0,MAX,0,{0},PTHREAD_MUTEX_INITIALIZER,PTHREAD_COND_INITIALIZER,PTHREAD_COND_INITIALIZER};

void*consumer(void*count)
{
    for(int i=0;i<10000;++i){
        pthread_mutex_lock(&product_queue.mutex);
        //if no product to use,then sleep and wait to be wake
        while(product_queue.used_space==0)
            pthread_cond_wait(&product_queue.consumer_cond,&product_queue.mutex);

        if(product_queue.buf[product_queue.consumer_index]!=product_queue.consumer_index)
            printf("buf[%d]=%d\n",product_queue.consumer_index,product_queue.buf[product_queue.consumer_index]);

        product_queue.consumer_index++;
        product_queue.consumer_index%=MAX;//this space can loop,no end spot

        product_queue.free_space++;
        product_queue.used_space--;
        pthread_mutex_unlock(&product_queue.mutex);
        //if producer wait for wake,then wake it
        if(product_queue.free_space>=1)
            pthread_cond_broadcast(&product_queue.product_cond);
    }
    return 0;
}

void*producer(void*count)
{
    for(;;){
    pthread_mutex_lock(&product_queue.mutex);
    //if no space to produce,then sleep to wait to be wake
    while(0==product_queue.free_space){
        pthread_cond_wait(&product_queue.product_cond,&product_queue.mutex);
    }
        //if product 10000,then quit
        if(10000==total){
            pthread_mutex_unlock(&product_queue.mutex);
            pthread_exit(NULL);
        }
    product_queue.buf[product_queue.producer_index]=product_queue.producer_index;
    total+=1;
    //test
    //printf("product %d times\n",total);
    *(int*)count+=1;

    //offset the pointer to the next position where product will be store
    product_queue.producer_index++;
    product_queue.producer_index%=MAX;

    //the free space minus one
    product_queue.free_space--;
    //the used space add one
    product_queue.used_space++;
    //must keep the sequence:unlock first,signal second
    pthread_mutex_unlock(&product_queue.mutex);
    //if consumer wait for wake,then wake it
    if(product_queue.used_space>0)
        pthread_cond_signal(&product_queue.consumer_cond);
    }
    return 0;
}
int main(void)
{
    pthread_t tid_producer[10],tid_consumer;
    //ensure have 11 thread create,but useless in linux
    //pthread_setconcurrency(11);
    //statistics the count of each producer product
    int count[10]={0};//this is statistics the product of each producer product
    //create 10 producer
    for(int i=0;i<10;++i)
        pthread_create(&tid_producer[i],NULL,producer,(void*)&count[i]);
    //create a consumer
    pthread_create(&tid_consumer,NULL,consumer,NULL);
    //wait for all product done
    for(int i=0;i<10;++i){
        pthread_join(tid_producer[i],NULL);
    }
    //echo the numeric of each producer product
    for(int i=0;i<10;++i)
        printf("thread %d:count:%d\n",i,count[i]);
    //wait for consumer
    pthread_join(tid_consumer,NULL);
    //echo the state of buffer
    printf("end:free_space:%d,used space:%d\n",product_queue.free_space,product_queue.used_space);
    return 0;
}
