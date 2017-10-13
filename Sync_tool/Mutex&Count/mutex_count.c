//one consumer,multi producer,sync by mutex and count
//

#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<errno.h>

#define MAX 1024
struct product{
    int producer_index;   //the next position producer will produce
    int consumer_index;   //the next position consumer will consume
    int free_space;       //unuse space
    int used_space;       //used space
    int buf[MAX];         //the buffer that store product
    pthread_mutex_t mutex;
    pthread_cond_t cond;
}product_queue={0,0,MAX,0,{0},PTHREAD_MUTEX_INITIALIZER,PTHREAD_COND_INITIALIZER};

void consumer(struct product*arg)
{
    for(int i=0;i<10000;++i){
        pthread_mutex_lock(&arg->mutex);
        //if no product to use,then sleep and wait to be wake
        if(arg->used_space==0)
            pthread_cond_wait(&arg->cond,&arg->mutex);

        if(arg->buf[arg->consumer_index]!=arg->consumer_index)
            printf("buf[%d]=%d\n",arg->consumer_index,arg->consumer_index);

        arg->consumer_index++;
        arg->consumer_index%=MAX;//this space can loop,no end spot

        arg->free_space++;
        arg->used_space--;
        pthread_mutex_unlock(&arg->mutex);
        //if producer wait for wake,then wake it
        if(arg->free_space==1)
            pthread_cond_signal(&arg->cond);
    }
    return ;
}

void producer(struct product*arg)
{
    for(int i=0;i<10000;++i){
        puts("producer");
        pthread_mutex_lock(&arg->mutex);
        //if no space to produce,then sleep to wait to be wake
        if(arg->free_space==0)
            pthread_cond_wait(&arg->cond,&arg->mutex);
        arg->buf[i]=i;

        arg->producer_index++;
        arg->producer_index%=MAX;

        arg->free_space--;
        arg->used_space++;
        pthread_mutex_unlock(&arg->mutex);
        //if consumer wait for wake,then wake it
        if(arg->used_space==1)
            pthread_cond_signal(&arg->cond);
    }
    return;
}
int main(void)
{
    producer(&product_queue);
    consumer(&product_queue);
    printf("end:free_space:%d,used space:%d\n",product_queue.free_space,product_queue.used_space);
    return 0;
}
