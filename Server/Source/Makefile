FLAGS= -std=c++11 -Wall -pthread -O3
DFLAGS= -std=c++11 -Wall -g -D DEBUG -pthread
CPPS = ../../Source/debugger.cpp ../../Source/connection.cpp ../../Source/semaphore.cpp server.cpp

server:
	g++ $(FLAGS) $(CPPS) ServerMain.cpp -o ServerMain.o
dserver:
	g++ $(DFLAGS) $(CPPS) ServerMain.cpp -o ServerMain.o
clean:
	rm *.o
