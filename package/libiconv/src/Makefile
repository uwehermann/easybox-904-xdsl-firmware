
SO_FILENAME = libiconv.so
SO_OUTPUT = $(SO_FILENAME).$(LIB_VER_MAJOR).$(LIB_VER_MINOR)
SO_LINK = -shared -Wl,-soname,$(SO_FILENAME).$(LIB_VER_MAJOR)
LIB_VER_MAJOR = 0
LIB_VER_MINOR = 1


LIB_CFLAGS  = $(CFLAGS) -shared -fPIC

OBJ = iconv.o
all: iconv

iconv: 
	$(CC) $(LIB_CFLAGS) -c iconv.c -o $(OBJ) -I./include
	$(CC) $(LIB_CFLAGS) $(SO_LINK) -o $(SO_OUTPUT) $(OBJ)
	ln -sf $(SO_OUTPUT)	$(SO_FILENAME).$(LIB_VER_MAJOR)
	ln -sf $(SO_FILENAME).$(LIB_VER_MAJOR)	$(SO_FILENAME)
 
clean:
	rm -rf $ $(SO_FILENAME)*