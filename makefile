# makefile
UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
    cc = clang++
else ifeq ($(UNAME), Darwin)
    cc = g++
else
    cc = g++-4.7
endif

all: dataserver client

reqchannel.o: reqchannel.h reqchannel.cpp
	$(cc) -c -g reqchannel.cpp

dataserver: dataserver.cpp reqchannel.o 
	$(cc) -o dataserver dataserver.cpp reqchannel.o -lpthread

client: client.cpp reqchannel.o
	$(cc) -o client client.cpp reqchannel.o
