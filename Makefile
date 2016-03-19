CC      = g++
CFLAGS  = -Wall
LDFLAGS = -lrt

all: server_start client_start receiver sender

server_start: server.o
	$(CC) -o $@ $^ $(LDFLAGS)

server.o: server.cpp 
	$(CC) -c $(CFLAGS) $<

client_start: client.o
	$(CC) -o $@ $^ $(LDFLAGS)

client.o: client.cpp 
	$(CC) -c $(CFLAGS) $<

sender: sender.o
	$(CC) -o $@ $^ $(LDFLAGS)

sender.o: sender.cpp 
	$(CC) -c $(CFLAGS) $<

receiver: receiver.o
	$(CC) -o $@ $^ $(LDFLAGS)

receiver.o: receiver.cpp 
	$(CC) -c $(CFLAGS) $<

.PHONY: clean

clean:
	rm server_start client_start sender receiver

