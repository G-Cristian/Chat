#include "../../Include/connection.h"
#include "../../Include/debugger.h"
#include "../Include/server.h"
#include <iostream>
#include <memory>
#include <string>

using namespace std;

int main(int argc, char *argv[]){
    if(argc < 2){
        cerr << "Not enough parameters." << endl;
        return 1;
    }

    Debugger::initInstance(std::cout, std::cerr);
    try{
        Server server(atoi(argv[1]));

        if(!server.start()){
            Debugger::getInstance()->printErr("Cannot listen");
        }
    }
    catch(string msg){
        Debugger::getInstance()->printErr(msg);
    }

    Debugger::dispose();

    return 0;
}