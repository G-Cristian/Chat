#include "../../Include/connection.h"
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

    Server server(atoi(argv[1]));

    if(!server.start()){
        cerr << "Cannot listen" << endl;
    }

    return 0;
}