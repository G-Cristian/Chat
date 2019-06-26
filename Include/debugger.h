#ifndef _DEBUGGER_H_
#define _DEBUGGER_H_

#include "../Include/semaphore.h"
#include <iostream>
#include <memory>
#include <string>

class Debugger{
public:
    static void initInstance(std::ostream &out, std::ostream &err);
    static std::shared_ptr<Debugger> getInstance();
    static void dispose();
    ~Debugger();

    template<typename T>
    void print(const T&);
    void print(const std::string&);
    void print(const char*);
    template<typename T>
    void printErr(const T&);
    void printErr(const std::string&);
    void printErr(const char*);
private:
    Debugger(std::ostream &out=std::cout, std::ostream &err=std::cerr);

    static std::shared_ptr<Debugger> _instance;
    std::ostream &_stdOut;
    std::ostream &_errOut;
    Mutex _stdOutMutex;
    Mutex _errOutMutex;
};

template<typename T>
void Debugger::print(const T &msg){
    ::lock(&_stdOutMutex);
    _stdOut << msg << std::endl;
    ::unlock(&_stdOutMutex);
}

template<typename T>
void Debugger::printErr(const T &msg){
    ::lock(&_errOutMutex);
    _errOut << msg << std::endl;
    ::unlock(&_errOutMutex);
}

#endif
