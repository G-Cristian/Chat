FLAGS= -std=c++11 -Wall -O3
DFLAGS= -std=c++11 -Wall -g -D DEBUG
SERVER_RELATIVE_PATH = ../Server/Source
CLIENT_RELATIVE_PATH = ../Client/Source

all: server client
	echo "done all"
dall: dserver dclient
	echo "done dall"
clean: serverclean clientclean
	echo "clean"
server:
	cd $(SERVER_RELATIVE_PATH) && make server && cd ../../Source
dserver:
	cd $(SERVER_RELATIVE_PATH) && make dserver && cd ../../Source
serverclean:
	cd $(SERVER_RELATIVE_PATH) && make clean && cd ../../Source
client:
	cd $(CLIENT_RELATIVE_PATH) && make client && cd ../../Source
dclient:
	cd $(CLIENT_RELATIVE_PATH) && make dclient && cd ../../Source
clientclean:
	cd $(CLIENT_RELATIVE_PATH) && make clean && cd ../../Source
