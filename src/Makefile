.PHONY: all clean

CFALGS=-Wall -g -shared -llua -fPIC

all: epoll.so socket.so

epoll.so: epoll.c epoll.h
	$(CC) epoll.c epoll.h -o $@ $(CFALGS)

socket.so: socket.c socket.h
	$(CC) socket.c socket.h -o $@ $(CFALGS)

clean:
	$(RM) *.so
