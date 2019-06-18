#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include <pthread.h>

typedef pthread_mutex_t Mutex;

int init(Mutex*);
int destroy(Mutex*);
int lock(Mutex*);
int unlock(Mutex*);


#endif
