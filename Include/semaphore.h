#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#if defined(__linux__) || defined(__unix__)
//linux based
#include <pthread.h>

typedef pthread_mutex_t Mutex;
typedef pthread_cond_t Conditional;
#elif defined(_WIN32) || defined(WIN32)
//windows based
#include <windows.h>

typedef CRITICAL_SECTION Mutex;
typedef CONDITION_VARIABLE Conditional;
#else
#error "OS not supperted."
#endif

int initMutex(Mutex*);
int destroyMutex(Mutex*);
int lock(Mutex*);
int unlock(Mutex*);

int initConditional(Conditional*);
int destroyConditional(Conditional*);
int wait(Conditional*, Mutex*);
int signal(Conditional*);

class Semaphore{
public:
    Semaphore(int value = 0);
    ~Semaphore();
    void signal();
    void wait();
private:
    int _value;
    int _wakeups;
    Mutex _mutex;
    Conditional _cond;
};

#endif
