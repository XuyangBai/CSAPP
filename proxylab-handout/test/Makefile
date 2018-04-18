# Makefile for Proxy Lab 
#
# You may modify this file any way you like (except for the handin
# rule). You instructor will type "make" on your specific Makefile to
# build your proxy from sources.

CC = gcc
CFLAGS = -g -Wall
LDFLAGS = -lpthread

all: client

csapp.o: csapp.c csapp.h
    $(CC) $(CFLAGS) -c csapp.c

client.o: client.c csapp.h
    $(CC) $(CFLAGS) -c client.c

client: client.o csapp.o
    $(CC) $(CFLAGS) client.o csapp.o -o client $(LDFLAGS)

