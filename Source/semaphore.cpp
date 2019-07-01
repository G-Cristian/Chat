#include "../Include/semaphore.h"

int initMutex(Mutex *mutex){
	int ret = -1;
#if defined(__linux__) || defined(__unix__)
    ret = pthread_mutex_init(mutex, NULL);
#elif defined(_WIN32) || defined(WIN32)
	ret = 1;
	InitializeCriticalSection(mutex);
	//TODO: Check errors
#else
#error "OS not supperted."
#endif
    return (ret == -1) ? 0 : 1;
}

int destroyMutex(Mutex *mutex){
	int ret = -1;
#if defined(__linux__) || defined(__unix__)
    ret = pthread_mutex_destroy(mutex);
#elif defined(_WIN32) || defined(WIN32)	
	ret = 1;
	DeleteCriticalSection(mutex);
	//TODO: Check errors
#else
#error "OS not supperted."
#endif
    return (ret == -1) ? 0 : 1;
}

int lock(Mutex *mutex){
	int ret = -1;
#if defined(__linux__) || defined(__unix__)
    ret = pthread_mutex_lock(mutex);
#elif defined(_WIN32) || defined(WIN32)
	ret = 1;
	EnterCriticalSection(mutex);
	//TODO: Check errors
#else
#error "OS not supperted."
#endif
    return (ret == -1) ? 0 : 1;
}

int unlock(Mutex *mutex){
	int ret = -1;
#if defined(__linux__) || defined(__unix__)
    ret = pthread_mutex_unlock(mutex);
#elif defined(_WIN32) || defined(WIN32)
	ret = 1;
	LeaveCriticalSection(mutex);
	//TODO: Check errors
#else
#error "OS not supperted."
#endif
    return (ret == -1) ? 0 : 1;
}

int initConditional(Conditional *cond){
	int ret = -1;
#if defined(__linux__) || defined(__unix__)
    ret = pthread_cond_init(cond, NULL);
#elif defined(_WIN32) || defined(WIN32)
	ret = 0;
	InitializeConditionVariable(cond);
	//TODO: Check errors
#else
#error "OS not supperted."
#endif
    return (ret != 0) ? 0 : 1;
}

int destroyConditional(Conditional *cond){
	int ret = -1;
#if defined(__linux__) || defined(__unix__)
    ret = pthread_cond_destroy(cond);
#elif defined(_WIN32) || defined(WIN32)
	ret = 0;
	//TODO: Check errors
#else
#error "OS not supperted."
#endif
    return (ret != 0) ? 0 : 1;
}

int wait(Conditional *cond, Mutex *mutex){
	int ret = -1;
#if defined(__linux__) || defined(__unix__)
    ret = pthread_cond_wait(cond, mutex);
#elif defined(_WIN32) || defined(WIN32)
	if(SleepConditionVariableCS(cond, mutex, INFINITE) != 0){
		//success
		ret = 0;
	}
	//TODO: Check errors
#else
#error "OS not supperted."
#endif
    return (ret != 0) ? 0 : 1;
}

int signal(Conditional *cond){
	int ret = -1;
#if defined(__linux__) || defined(__unix__)
    ret = pthread_cond_signal(cond);
#elif defined(_WIN32) || defined(WIN32)
	ret = 0;
	WakeConditionVariable(cond);
	//TODO: Check errors
#else
#error "OS not supperted."
#endif
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