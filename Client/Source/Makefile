FLAGS= -std=c++11 -Wall -pthread -O3
DFLAGS= -std=c++11 -Wall -g -D DEBUG -pthread
CPPS = ../../Source/connection.cpp ../../Source/semaphore.cpp

client:
	g++ $(FLAGS) $(CPPS) ClientMain.cpp -o ClientMain.o
dclient:
	g++ $(DFLAGS) $(CPPS) ClientMain.cpp -o ClientMain.o
clean:
	rm *.o
