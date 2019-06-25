#include "../Include/semaphore.h"
#include <pthread.h>

int initMutex(Mutex *mutex){
    int ret = pthread_mutex_init(mutex, NULL);
    return (ret == -1) ? 0 : 1;
}

int destroyMutex(Mutex *mutex){
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

int initConditional(Conditional *cond){
    int ret = pthread_cond_init(cond, NULL);
    return (ret != 0) ? 0 : 1;
}

int destroyConditional(Conditional *cond){
    int ret = pthread_cond_destroy(cond);
    return (ret != 0) ? 0 : 1;
}

int wait(Conditional *cond, Mutex *mutex){
    int ret = pthread_cond_wait(cond, mutex);
    return (ret != 0) ? 0 : 1;
}

int signal(Conditional *cond){
    int ret = pthread_cond_signal(cond);
    return (ret != 0) ? 0 : 1;
}

Semaphore::Semaphore(int value /*=0*/):
    _value(value),
    _wakeups(0){
    ::initMutex(&_mutex);
    ::initConditional(&_cond);
}

Semaphore::~Semaphore(){
    ::destroyConditional(&_cond);
    ::destroyMutex(&_mutex);
}

void Semaphore::signal(){
    ::lock(&_mutex);

    ++_value;
    if(_value <= 0){
        ++_wakeups;
        ::signal(&_cond);
    }

    ::unlock(&_mutex);
}

void Semaphore::wait(){
    ::lock(&_mutex);

    --_value;
    if(_value < 0){
        do{
            ::wait(&_cond,&_mutex);
        }while(_wakeups < 1);
        --_wakeups;
    }

    ::unlock(&_mutex);
}