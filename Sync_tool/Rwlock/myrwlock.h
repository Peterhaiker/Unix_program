#ifndef MYRWLOCK
#define MYRWLOCK
typedef struct{
    pthread_mutex_t mutex;
    pthread_cond_t rdcont,wrcont;
    int rdwaiters;//number of wait for read
    int wrwaiters;//number of wait for write
    int rw_state;//-1 is writer,else is reader
    int init_state;//0 is not init,1 is init
}Pthread_rwlock_t;
typedef int Pthread_rwlockattr_t;
#define PTHREAD_RWLOCK_INITIALIZER {PTHREAD_MUTEX_INITIALIZER,PTHREAD_COND_INITIALIZER,PTHREAD_COND_INITIALIZER,0,0,0,1}
int Pthread_rwlock_init(Pthread_rwlock_t *,Pthread_rwlockattr_t*);
int Pthread_rwlock_destroy(Pthread_rwlock_t*);
int Pthread_rwlock_rdlock(Pthread_rwlock_t*);
int Pthread_rwlock_wrlock(Pthread_rwlock_t*);
int Pthread_rwlock_tryrdlock(Pthread_rwlock_t*);
int Pthread_rwlock_trywrlock(Pthread_rwlock_t*);
int Pthread_rwlock_unlock(Pthread_rwlock_t*);
void pthread_cancelrd(void*);
void pthread_cancelwr(void*);
#endif
