#include "../Include/debugger.h"
#include <utility>

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

std::ostream& Debugger::printAux(std::ostream &outstream, const std::string &val)const{
    #ifdef DEBUG
    outstream << "std::ostream& Debugger::printAux(std::ostream &outstream, const std::string &val)const" << std::endl;
    #endif
    return outstream << "\"" << val << "\"";
}

std::ostream& Debugger::printAux(std::ostream &outstream, std::nullptr_t val)const{
    #ifdef DEBUG
    outstream << "std::ostream& Debugger::printAux(std::ostream &outstream, std::nullptr_t val)const" << std::endl;
    #endif
    return outstream << "nullptr";
}

std::ostream& Debugger::printAux(std::ostream &outstream, const char *val)const{
    #ifdef DEBUG
    outstream << "std::ostream& Debugger::printAux(std::ostream &outstream, const char *val)const" << std::endl;
    #endif
    if(val){
        return printAux(outstream, std::string(val));
    }
    else{
        return outstream << "NULL";
    }
}

std::ostream& Debugger::printAux(std::ostream &outstream, char *val)const{
    #ifdef DEBUG
    outstream << "std::ostream& Debugger::printAux(std::ostream &outstream, char *val)const" << std::endl;
    #endif
    if(val){
        return printAux(outstream, std::string(val));
    }
    else{
        return outstream << "NULL";
    }
}