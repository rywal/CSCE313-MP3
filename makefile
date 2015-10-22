# makefile

all: dataserver client

reqchannel.o: reqchannel.h reqchannel.cpp
	clang++ -c -g reqchannel.cpp

dataserver: dataserver.cpp reqchannel.o 
	clang++ -o dataserver dataserver.cpp reqchannel.o -lpthread

client: client.cpp reqchannel.o
	clang++ -o client client.cpp reqchannel.o
