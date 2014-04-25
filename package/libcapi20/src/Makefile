
LIBNAME = libcapi20.so

SRCS =  capi20.c \
		capifunc.c \
		convert.c

OBJS = $(SRCS:.c=.o)

.SUFFIXES: .c .o

all: depend $(LIBNAME)

$(OBJS): %.o: %.c
	$(CC) -c $(CFLAGS) $(OPTFLAGS) -o $@ $<

$(LIBNAME): $(OBJS)
	$(CC) -shared -Wl,-soname,$(LIBNAME) $(OBJS) \
	  $(LDFLAGS) $(EXTRALIBS) -o $(LIBNAME)

clean:
	-$(RM) -f *.o *.so*
	-$(RM) -f .depend

depend:
	$(CC) -MM $(CFLAGS) $(SRCS) 1>.depend

.PHONY: clean depend
