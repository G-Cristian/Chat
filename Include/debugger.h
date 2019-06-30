#ifndef _DEBUGGER_H_
#define _DEBUGGER_H_

#include "../Include/semaphore.h"
#include <iostream>
#include <memory>
#include <string>
#include <utility>

class Debugger{
public:
    static void initInstance(std::ostream &out, std::ostream &err);
    static std::shared_ptr<Debugger> getInstance();
    static void dispose();
    ~Debugger();

    template<typename T>
    void print(T&&);
    template<typename T, typename... Args>
    void print(T&&, Args&&... rest);
    
    template<typename T>
    void printErr(T&&);
    template<typename T, typename... Args>
    void printErr(T&&, Args&&... rest);
private:
    Debugger(std::ostream &out=std::cout, std::ostream &err=std::cerr);
    template<typename T>
    std::ostream& printAux(std::ostream&, const T&)const;
    template<typename T>
    std::ostream& printAux(std::ostream&, T*)const;
    std::ostream& printAux(std::ostream&, const std::string&)const;
    std::ostream& printAux(std::ostream&, std::nullptr_t)const;
    std::ostream& printAux(std::ostream&, const char*)const;
    std::ostream& printAux(std::ostream&, char*)const;

    static std::shared_ptr<Debugger> _instance;
    std::ostream &_stdOut;
    std::ostream &_errOut;
    Mutex _stdOutMutex;
    Mutex _errOutMutex;
};

template <typename T>
void Debugger::print(T &&msg){
    ::lock(&_stdOutMutex);
    #ifdef DEBUG
    _stdOut << "void Debugger::print(T &&msg)" << std::endl;
    #endif
    printAux(_stdOut, std::forward<T>(msg));
    _stdOut << std::endl;
    ::unlock(&_stdOutMutex);
}
template <typename T, typename... Args>
void Debugger::print(T &&msg, Args&&... rest){
    ::lock(&_stdOutMutex);
    #ifdef DEBUG
    _stdOut << "void Debugger::print(T &&msg, Args&&... rest)" << std::endl;
    #endif
    printAux(_stdOut, std::forward<T>(msg));
    ::unlock(&_stdOutMutex);
    print(std::forward<Args>(rest)...);
}

template <typename T>
void Debugger::printErr(T &&msg){
    ::lock(&_errOutMutex);
    #ifdef DEBUG
    _errOut << "void Debugger::printErr(T &&msg)" << std::endl;
    #endif
    printAux(_errOut, std::forward<T>(msg));
    _errOut << std::endl;
    ::unlock(&_errOutMutex);
}
template <typename T, typename... Args>
void Debugger::printErr(T &&msg, Args&&... rest){
    ::lock(&_errOutMutex);
    #ifdef DEBUG
    _errOut << "void Debugger::printErr(T &&msg, Args&&... rest)" << std::endl;
    #endif
    printAux(_errOut, std::forward<T>(msg));
    ::unlock(&_errOutMutex);
    printErr(std::forward<Args>(rest)...);
}

template<typename T>
std::ostream& Debugger::printAux(std::ostream &outstream, const T &val)const{
    #ifdef DEBUG
    outstream << "std::ostream& Debugger::printAux(std::ostream &outstream, const T &val)const" << std::endl;
    #endif
    return outstream << val;
}
template<typename T>
std::ostream& Debugger::printAux(std::ostream &outstream, T *val)const{
    #ifdef DEBUG
    outstream << "std::ostream& Debugger::printAux(std::ostream &outstream, T *val)const" << std::endl;
    #endif
    if(val){
        return outstream << "val: " << val << ", *val: " << *val;
    }
    else{
        return outstream << "NULL";
    }
}

#endif
