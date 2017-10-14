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
    //test
    puts("consumer");
    for(int i=0;i<10000;++i){
        pthread_mutex_lock(&product_queue.mutex);
        //if no product to use,then sleep and wait to be wake
        if(product_queue.used_space==0)
            pthread_cond_wait(&product_queue.consumer_cond,&product_queue.mutex);

        if(product_queue.buf[product_queue.consumer_index]!=product_queue.consumer_index)
            printf("buf[%d]=%d\n",product_queue.consumer_index,product_queue.buf[product_queue.consumer_index]);

        //test
        printf("consumer:%d times\n",i+1);
        product_queue.consumer_index++;
        product_queue.consumer_index%=MAX;//this space can loop,no end spot

        product_queue.free_space+=1;
        product_queue.used_space-=1;
        if(product_queue.free_space>MAX||product_queue.used_space<0)
            printf("free:%d,used:%d\n",product_queue.free_space,product_queue.used_space);
        pthread_mutex_unlock(&product_queue.mutex);
        //if producer wait for wake,then wake it
        if(product_queue.free_space==1)
            pthread_cond_broadcast(&product_queue.product_cond);
    }
    return 0;
}

void*producer(void*count)
{
    //test
    puts("product");
    for(;;){
    //a total of 10000 production
    pthread_mutex_lock(&product_queue.mutex);
    if(10000==total){
        pthread_mutex_unlock(&product_queue.mutex);
        return 0;
    }
    //if no space to produce,then sleep to wait to be wake
    if(0==product_queue.free_space)
        pthread_cond_wait(&product_queue.product_cond,&product_queue.mutex);
    product_queue.buf[product_queue.producer_index]=product_queue.producer_index;
    total+=1;
    printf("product %d times\n",total);
    *(int*)count+=1;

    //offset the pointer to the next position where product will be store
    product_queue.producer_index++;
    product_queue.producer_index%=MAX;

    //the free space minus one
    product_queue.free_space-=1;
    //the used space add one
    product_queue.used_space+=1;
    if(product_queue.free_space<0||product_queue.used_space>MAX)
        printf("free:%d,used:%d\n",product_queue.free_space,product_queue.used_space);
    //must keep the sequence:unlock first,signal second
    pthread_mutex_unlock(&product_queue.mutex);
    //if consumer wait for wake,then wake it
    if(product_queue.used_space==1)
        pthread_cond_signal(&product_queue.consumer_cond);
    }
    return 0;
}
int main(void)
{
    pthread_t tid_producer[10],tid_consumer;
    //ensure have two thread create
//    pthread_setconcurrency(0);
    pthread_setconcurrency(11);
    //test
    puts("a");
    int count[10]={0}; //statistics the count of each producer product
    for(int i=0;i<10;++i){
        //test
        printf("create %d\n",i);
        pthread_create(&tid_producer[i],NULL,producer,(void*)&count[i]);
    }
    //test
    puts("b");
    pthread_create(&tid_consumer,NULL,consumer,NULL);
    for(int i=0;i<10;++i){
        pthread_join(tid_producer[i],NULL);
        printf("thread %d:count:%d\n",i,count[i]);
    }
    pthread_join(tid_consumer,NULL);
    printf("end:free_space:%d,used space:%d\n",product_queue.free_space,product_queue.used_space);
    return 0;
}
