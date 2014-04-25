
#ifneq ($(LIB_MAPI_SRCS),)
#	$$(error no API source files )
#endif
#ifeq ($(LIB_HDL_SRCS),)
#	$$(error no handler source files )
#endif
#ifneq ($(PKG_TEST_CLI),)
#ifeq ($(LIB_TEST_SRCS),)
#	$$(error no test program source files )
#endif
#endif


####################################################################################

LIB_MOD = dsl
ifneq ($(ARC_MOD_NAME),)
	LIB_MOD = $(ARC_MOD_NAME)
endif

MAJOR_VER = 0
ifneq ($(PKG_VERSION_MAJOR),)
	MAJOR_VER = $(PKG_VERSION_MAJOR)
endif

MINOR_VER = 1
ifneq ($(PKG_VERSION_MINOR),)
	MINOR_VER = $(PKG_VERSION_MINOR)
endif

LIB_MAPI_NAME =
ifneq ($(LIB_MAPI_SRCS),)
LIB_MAPI_NAME = libmapi_$(LIB_MOD).so
endif

LIB_HDL_NAME  = libmhdl_$(LIB_MOD).so
#LIB_TEST_NAME = libmapi_$(LIB_MOD)_cli
LIB_TEST_NAME =
ifneq ($(PKG_TEST_CLI),)
	LIB_TEST_NAME = $(PKG_TEST_CLI)
endif


####################################################################################
ifeq ($(DEBUG),1)
	LIB_CFLAGS       = -DMID_DEBUG
else
	LIB_CFLAGS       =
endif
ifeq ($(MAPFILE),1)
ifneq ($(LIB_MAPI_SRCS),)
	LIB_MAPI_LDFLAGS = -Wl,-Map,$(LIB_MAPI_NAME).map
endif
	LIB_HDL_LDFLAGS  = -Wl,-Map,$(LIB_HDL_NAME).map
	LIB_TEST_LDFLAGS = -Wl,-Map,$(LIB_TEST_NAME).map
else
	LIB_MAPI_LDFLAGS =
	LIB_HDL_LDFLAGS  =
	LIB_TEST_LDFLAGS =
endif

LIB_CFLAGS       += $(CFLAGS) -shared -fPIC
LIB_MAPI_CFLAGS  += $(LIB_CFLAGS) -f $(STAGING_DIR)/usr/lib/libmapitrnx.so
LIB_MAPI_LDFLAGS += $(LDFLAGS) -Wl,-soname,$(LIB_MAPI_NAME).$(MAJOR_VER).$(MINOR_VER) $(PKG_MAPI_LDFLAGS)
LIB_HDL_LDFLAGS  += $(LDFLAGS) -Wl,-soname,$(LIB_HDL_NAME).$(MAJOR_VER).$(MINOR_VER) $(PKG_HDL_LDFLAGS)
LIB_TEST_LDFLAGS += -L./ -lmapitrnx -lmapi_$(LIB_MOD) $(PKG_TEST_CLI_LDFLAGS) $(LDFLAGS)

LIB_MAPI_OBJS = $(LIB_MAPI_SRCS:.c=.o)
LIB_HDL_OBJS  = $(LIB_HDL_SRCS:.c=.o)
ifneq ($(LIB_TEST_NAME),)
	LIB_TEST_OBJS = $(LIB_TEST_SRCS:.c=.o)
endif

.SUFFIXES: .c .o

LIB_DEPEND_NAME = 
ifneq ($(LIB_MAPI_SRCS),)
LIB_DEPEND_NAME += $(LIB_MAPI_NAME)
endif
ifneq ($(LIB_HDL_SRCS),)
LIB_DEPEND_NAME += $(LIB_HDL_NAME)
endif
LIB_DEPEND_NAME += $(LIB_TEST_NAME)

all: depend $(LIB_DEPEND_NAME)

$(LIB_MAPI_OBJS) $(LIB_HDL_OBJS): %.o: %.c
	$(CC) -c $(LIB_CFLAGS) -o $@ $<

ifneq ($(LIB_MAPI_SRCS),)
$(LIB_MAPI_NAME): $(LIB_MAPI_OBJS)
	$(CC) $(LIB_MAPI_CFLAGS) $(LIB_MAPI_LDFLAGS) \
		-o $(LIB_MAPI_NAME).$(MAJOR_VER).$(MINOR_VER) $(LIB_MAPI_OBJS)
	ln -sf $(LIB_MAPI_NAME).$(MAJOR_VER).$(MINOR_VER) $(LIB_MAPI_NAME)
	ln -sf $(LIB_MAPI_NAME).$(MAJOR_VER).$(MINOR_VER) $(LIB_MAPI_NAME).$(MAJOR_VER)
endif

$(LIB_HDL_NAME): $(LIB_HDL_OBJS)
	$(CC) $(LIB_CFLAGS) $(LIB_HDL_LDFLAGS) \
		-o $(LIB_HDL_NAME).$(MAJOR_VER).$(MINOR_VER) $(LIB_HDL_OBJS)
	ln -sf $(LIB_HDL_NAME).$(MAJOR_VER).$(MINOR_VER) $(LIB_HDL_NAME)
	ln -sf $(LIB_HDL_NAME).$(MAJOR_VER).$(MINOR_VER) $(LIB_HDL_NAME).$(MAJOR_VER)

ifneq ($(LIB_TEST_NAME),)

$(LIB_TEST_OBJS): %.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(LIB_TEST_NAME): $(LIB_TEST_OBJS) $(LIB_MAPI_NAME)
	$(CC) $(CFLAGS) $(LIB_TEST_LDFLAGS) -o $(LIB_TEST_NAME) $(LIB_TEST_OBJS)

endif

clean:
	rm -f $(LIB_MAPI_NAME) $(LIB_HDL_NAME) $(LIB_TEST_NAME)
	rm -f *.o
	rm -f $(LIB_MAPI_NAME).depend $(LIB_HDL_NAME).depend

depend:
ifneq ($(LIB_MAPI_SRCS),)
	$(CC) -MM $(LIB_CFLAGS) $(LIB_MAPI_SRCS) 1>$(LIB_MAPI_NAME).depend
endif
ifneq ($(LIB_HDL_SRCS),)
	$(CC) -MM $(LIB_CFLAGS) $(LIB_HDL_SRCS)  1>$(LIB_HDL_NAME).depend
endif

.PHONY: clean depend
