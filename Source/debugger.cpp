#include "../Include/debugger.h"

std::shared_ptr<Debugger> Debugger::_instance = nullptr;

Debugger::Debugger(std::ostream &out/*=std::cout*/, std::ostream &err/*=std::cerr*/):
    _stdOut(out),
    _errOut(err){
    ::initMutex(&_stdOutMutex);
    ::initMutex(&_errOutMutex);
}
Debugger::~Debugger(){
    ::destroyMutex(&_errOutMutex);
    ::destroyMutex(&_stdOutMutex);
}

void Debugger::initInstance(std::ostream &out, std::ostream &err){
    if(_instance == nullptr){
        _instance=std::shared_ptr<Debugger>(new Debugger(out, err));
    }
}
std::shared_ptr<Debugger> Debugger::getInstance(){
    if(_instance == nullptr){
        _instance=std::shared_ptr<Debugger>(new Debugger());
    }

    return _instance;
}
void Debugger::dispose(){
    _instance = nullptr;
}

void Debugger::print(const std::string &msg){
    ::lock(&_stdOutMutex);
    _stdOut << msg << std::endl;
    ::unlock(&_stdOutMutex);
}

void Debugger::print(const char *msg){
    ::lock(&_stdOutMutex);
    _stdOut << msg << std::endl;
    ::unlock(&_stdOutMutex);
}

void Debugger::printErr(const std::string &msg){
    ::lock(&_errOutMutex);
    _errOut << msg << std::endl;
    ::unlock(&_errOutMutex);
}

void Debugger::printErr(const char *msg){
    ::lock(&_errOutMutex);
    _errOut << msg << std::endl;
    ::unlock(&_errOutMutex);
}