/*---------------------------------------------
 * myrwlock.c
 * Copyright (C) 2017-10-15 12:56 
 *author Peterhaiker 
 *mail   <vim.memory@gmail.com>
 *----------------------------------------------
 *Description:
 */

#include<stdio.h>
#include<pthread.h>
#include<errno.h>
#include"myrwlock.h"

int Pthread_rwlock_init(Pthread_rwlock_t*rw,Pthread_rwlockattr_t*attr)
{
    int err=0;
    if((err=pthread_mutex_init(&rw->mutex,NULL))!=0)
        goto err1;
    if((err=pthread_cond_init(&rw->rdcont,NULL))!=0)
        goto err2;
    if((err=pthread_cond_init(&rw->wrcont,NULL))!=0)
        goto err3;
    rw->rdwaiters=rw->wrwaiters=rw->rw_state=0;
    rw->init_state=1;
    
err3:
    pthread_cond_destroy(&rw->rdcont);
err2:
    pthread_mutex_destroy(&rw->mutex);
err1:
    return err;
}

int Pthread_rwlock_destroy(Pthread_rwlock_t*rw)
{
    if(0==rw->init_state){
        puts("error:not init");
        return(EINVAL);
    }
    if(rw->rdwaiters!=0||rw->wrwaiters!=0){
        puts("lock is in use");
        return(EBUSY);
    }
    pthread_mutex_destroy(&rw->mutex);
    pthread_cond_destroy(&rw->rdcont);
    pthread_cond_destroy(&rw->wrcont);
    rw->init_state=0;
    return 0;
}

int Pthread_rwlock_rdlock(Pthread_rwlock_t*rw)
{
    if(0==rw->init_state){
        puts("lock not init");
        return(EINVAL);
    }
    //get the mutex to change the date in lock struct
    int result=0;
    if((result=pthread_mutex_lock(&rw->mutex))!=0){
        return(result);
    }
    //if writer is running
    //the comment is writer priority
    while(-1==rw->rw_state/*||0<rw->wrwaiters*/){
        pthread_cleanup_push(pthread_cancelrd,(void*)rw);
        rw->rdwaiters++;
        result=pthread_cond_wait(&rw->rdcont,&rw->mutex);
        pthread_cleanup_pop(0);
        rw->rdwaiters--;
        if(0!=result)
            break;
    }
    if(0==result)
        rw->rw_state++;
    pthread_mutex_unlock(&rw->mutex);
    return(result);
}


int Pthread_rwlock_tryrdlock(Pthread_rwlock_t*rw)
{
    if(0==rw->init_state){
        puts("not init lock");
        return(EINVAL);
    }
    int result=0;
    if((result=pthread_mutex_lock(&rw->mutex))!=0)
        return(result);
    //the comment is writer priority
    if(-1==rw->rw_state/*||0<rw->wrwaiters*/)
        return(EBUSY);
    else
        rw->rdwaiters++;
    pthread_mutex_unlock(&rw->mutex);
    return(result);
}

int Pthread_rwlock_wrlock(Pthread_rwlock_t*rw)
{
    if(0==rw->init_state){
        puts("lock not init");
        return(EINVAL);
    }
    int result=0;
    if((result=pthread_mutex_lock(&rw->mutex))!=0)
        return(result);
    while(rw->rw_state!=0){
        pthread_cleanup_push(pthread_cancelwr,(void*)rw);
        rw->wrwaiters++;
        result=pthread_cond_wait(&rw->wrcont,&rw->mutex);
        pthread_cleanup_pop(0);
        rw->wrwaiters--;
        if(0!=result)
            break;
    }
    if(0==result)
        rw->rw_state--;
    pthread_mutex_unlock(&rw->mutex);
    return(result);
}

int Pthread_rwlock_trywrlock(Pthread_rwlock_t*rw)
{
    if(0==rw->init_state){
        puts("lock not init");
        return(EINVAL);
    }
    int result=0;
    if((result=pthread_mutex_lock(&rw->mutex))!=0)
        return(result);
    if(rw->rw_state!=0){
        pthread_mutex_unlock(&rw->mutex);
        return(EBUSY);
    }
    else
        rw->rw_state--;
    pthread_mutex_unlock(&rw->mutex);
    return result;
}

int Pthread_rwlock_unlock(Pthread_rwlock_t*rw)
{
    if(0==rw->init_state){
        puts("lock not init");
        return(EINVAL);
    }
    int result=0;
    if((result=pthread_mutex_lock(&rw->mutex))!=0)
        return(result);
    if(0==rw->rw_state)
        puts("lock already locked");
    else if(-1==rw->rw_state)
        rw->rw_state++;
    else
        rw->rw_state--;
    //now,writer priority,so wake it if has writer sleep
    /*if(0<rw->wrwaiters){*/
    if(0<rw->rdwaiters)
        //test whether have readers running
        result=pthread_cond_broadcast(&rw->rdcont);
        /*if(rw->rw_state==0)
            result=pthread_cond_signal(&rw->wrcont);
    }*/
    //maybe writer will be hungry
    //
    else if(0</*rw->rdwaiters*/rw->wrwaiters)
        //result=pthread_cond_broadcast(&rw->rdcont);
        result=pthread_cond_signal(&rw->wrcont);
    pthread_mutex_unlock(&rw->mutex);
    return(result);
}

void pthread_cancelrd(void*arg)
{
    Pthread_rwlock_t*rw=(Pthread_rwlock_t*)arg;
    rw->rdwaiters--;
    pthread_mutex_unlock(&rw->mutex);
}

void pthread_cancelwr(void*arg)
{
    Pthread_rwlock_t*rw=(Pthread_rwlock_t*)arg;
    rw->wrwaiters--;
    pthread_mutex_unlock(&rw->mutex);
}
