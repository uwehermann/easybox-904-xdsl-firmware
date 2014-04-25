# This is the makefile for npio2.

.PHONY: all clean

all: noip2

noip2: noip2.o
	$(CC) $(CFLAGS) $(LDFLAGS) -O2 -o $@ $^

.o: .c 
	$(CC) $(CFLAGS) -O2 -c -o $@ $^

clean:
	rm -f *.o noip2
