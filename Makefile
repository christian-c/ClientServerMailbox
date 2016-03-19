C       = gcc
CC      = g++
CFLAGS  = -Wall -Wformat -fpermissive -std=c++11 
LDFLAGS = -lrt -lmsgpack

all: server client msgpack_test

server: server.o
	$(CC) -o $@ $^ $(LDFLAGS)

server.o: server.cpp
	$(CC) -c $(CFLAGS) $<

client: client.o
	$(CC) -o $@ $^ $(LDFLAGS)

client.o: client.cpp
	$(CC) -c $(CFLAGS) $<

msgpack_test.o: msgpack_test.cpp
	$(CC) -c $(CFLAGS) $<

msgpack_test: msgpack_test.o
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean

clean:
	rm server client

