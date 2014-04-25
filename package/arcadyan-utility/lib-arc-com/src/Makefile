
LIBNAME = libArcCom.so

SRCS =  libArcComUtil.c \
		libArcComOs.c \
		libArcComNet.c \
		libArcComDrv.c \
		libArcComCfg.c \
		libArcComTmr.c \
		libArcComMd5.c \

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
