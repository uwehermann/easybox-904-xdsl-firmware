# This is the makefile for dtdnsupdate.

.PHONY: all clean

all: dtdnsupdate

dtdnsupdate: dtdnsupdate.o
	$(CC) $(CFLAGS) $(LDFLAGS) -O2 -o $@ $^

.o: .c 
	$(CC) $(CFLAGS) -O2 -c -o $@ $^

clean:
	rm -f *.o dtdnsupdate
