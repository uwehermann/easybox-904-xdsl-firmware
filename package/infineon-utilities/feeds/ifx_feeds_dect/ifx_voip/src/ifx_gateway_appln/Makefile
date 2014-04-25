export KERNEL=$(LINUX_DIR) 
export SHELL=/usr/bin/env bash -c

#dirs := voip_ifx_ixml voip_ifx_network_agent voip_ifx_rtp voip_ifx_fax voip_ifx_sip voip_ifx_cgi voip_ifx_vmapi 
dirs := voip_ifx_ixml voip_ifx_network_agent voip_ifx_rtp voip_ifx_fax voip_ifx_sip voip_ifx_cgi

ifeq ($(DECT_AGENT_DIR),1)                                                     
dirs += dect_ifx_agent
endif

dirs += voip_ifx_gateway_appln

all:
	for i in $(dirs) ; do \
   if test $$i == "voip_ifx_fax" -a -d $(IFX_PRODUCT_ROOT)/$$i/make ; then \
    $(MAKE) -C $(IFX_PRODUCT_ROOT)/voip_ifx_fax/make all IFX_CFLAGS="$(IFX_CFLAGS)" IFX_LDFLAGS="$(IFX_LDFLAGS)" LINUX_DIR=$(LINUX_DIR) $(TARGET_CONFIGURE_OPTS) || exit;\
   elif test $$i == "voip_ifx_sip" -a -d $(IFX_PRODUCT_ROOT)/$$i/make ; then \
    $(MAKE) -C $(IFX_PRODUCT_ROOT)/voip_ifx_sip/make all IFX_CFLAGS="$(IFX_CFLAGS)" IFX_LDFLAGS="$(IFX_LDFLAGS)" LINUX_DIR=$(LINUX_DIR) $(TARGET_CONFIGURE_OPTS) || exit;\
	elif [ "$$i" != "voip_ifx_sip" -a "$$i" != "voip_ifx_fax" ]; then \
    $(MAKE) -C $(IFX_PRODUCT_ROOT)/$$i/make all IFX_CFLAGS="$(IFX_CFLAGS)" IFX_LDFLAGS="$(IFX_LDFLAGS)" LINUX_DIR=$(LINUX_DIR) $(TARGET_CONFIGURE_OPTS) || exit;\
   fi \
  done      
  
clean:
	for i in $(dirs) ; do \
   if test $$i == "voip_ifx_fax" -a -d $(IFX_PRODUCT_ROOT)/$$i/make ; then \
    $(MAKE) -C $(IFX_PRODUCT_ROOT)/voip_ifx_fax/make clean IFX_CFLAGS="$(IFX_CFLAGS)" IFX_LDFLAGS="$(IFX_LDFLAGS)" LINUX_DIR=$(LINUX_DIR) $(TARGET_CONFIGURE_OPTS);\
   elif test $$i == "voip_ifx_sip" -a -d $(IFX_PRODUCT_ROOT)/$$i/make ; then \
    $(MAKE) -C $(IFX_PRODUCT_ROOT)/voip_ifx_sip/make clean IFX_CFLAGS="$(IFX_CFLAGS)" IFX_LDFLAGS="$(IFX_LDFLAGS)" LINUX_DIR=$(LINUX_DIR) $(TARGET_CONFIGURE_OPTS);\
	elif [ "$$i" != "voip_ifx_sip" -a "$$i" != "voip_ifx_fax" ]; then \
    $(MAKE) -C $(IFX_PRODUCT_ROOT)/$$i/make clean IFX_CFLAGS="$(IFX_CFLAGS)" IFX_LDFLAGS="$(IFX_LDFLAGS)" LINUX_DIR=$(LINUX_DIR) $(TARGET_CONFIGURE_OPTS);\
   fi \
  done      
	
