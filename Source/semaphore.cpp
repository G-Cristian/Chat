#include "../Include/semaphore.h"
#include <pthread.h>

int init(Mutex *mutex){
    int ret = pthread_mutex_init(mutex, NULL);
    return (ret == -1) ? 0 : 1;
}

int destroy(Mutex *mutex){
    int ret = pthread_mutex_destroy(mutex);
    return (ret == -1) ? 0 : 1;
}

int lock(Mutex *mutex){
    int ret = pthread_mutex_lock(mutex);
    return (ret == -1) ? 0 : 1;
}

int unlock(Mutex *mutex){
    int ret = pthread_mutex_unlock(mutex);
    return (ret == -1) ? 0 : 1;
}
