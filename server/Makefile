CC = gcc
CFLAGS = -Wall -g
LIBS = -lpthread

BIN = server 
OBJS = server.c linkedlist.c 

all: $(OBJS)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJS) $(LIBS)

clean:
	rm $(BIN) 

