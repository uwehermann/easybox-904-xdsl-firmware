# Makefile for Linux driver
# Different rules/options for 2.4 and 2.6 kernels
ifeq ("$(PATCHLEVEL)","4")
# 2.4
# Makefile for the INCA-IP2 specific kernel interface routines under Linux.
#
#KERNEL = 
#KERNEL = /export/GAN/linux/linux-2.4.31

CROSS	=	mips-linux-
CC	=	$(CROSS)gcc
LD	= 	$(CROSS)ld
STRIP	=	$(CROSS)strip
INCDIRS =   -I./ -I$(KERNEL)/include	-I$(IFX_MAPI_DIR)/include -I$(IFX_PRODUCT_ROOT)/dect_ifx_catiq_stack -I$(TAPI_HEADER) -I$(TAPI_KPI_HEADER) -I$(TAPI_KPI_HEADER)/lib/lib_bufferpool/

#EXTFLAGS = -DIFX_SW_SAMURAI -DCONFIG_SWITCH_ADM6996_MDIO #-DDEBUG #-gstabs+
#EXTFLAGS = -DIFX_SW_TANTOS -DCONFIG_SWITCH_ADM6996_MDIO #-DDEBUG #-gstabs+
EXTFLAGS = -DFT -DCONFIG_SWITCH_MDIO_ACC -DCONFIG_TWINPASS $(VDSL2)#-DDEBUG #-gstabs+
MIPSFLAGS =	-Wall -Wstrict-prototypes -Wno-trigraphs -O2 -fomit-frame-pointer -fno-strict-aliasing -fno-common -G 0 -mno-abicalls -fno-pic -march=4kc -mips32 -Wa,--trap -pipe -mlong-calls -DTAPI_LOOPBACK
CFLAGS = $(INCDIRS) -DEXPORT_SYMTAB -DMODULE -D__KERNEL__ $(EXTFLAGS) $(MIPSFLAGS)

SRC := drv_dect.c fhmac.c fmbc.c drv_dect_cosic_drv.c drv_dect_cosic_kthread.c fdebug.c

TARGET := drv_dect


CFLAGS += -DFT -DCATIQ -DDECT_NG -DDECT_NG_AS -DDECT_NG_WBS -DDECT_NG_FULL_PAGE
CFLAGS += -DCATIQ_UPLANE 

all: $(TARGET)

$(TARGET): $(SRC:.c=.o)
	$(LD) -r $^ -o $@

clean:
	rm -f *.o $(TARGET)

%.o:%.c
	$(CC)  -c $< $(CFLAGS) -o $@
else
# 2.6
#KERNEL = /home/radva/DANUBE_BSP26_2.2.0//source/kernel/opensource/linux-2.6/
EXTRA_CFLAGS :=  -I$(src)
EXTRA_CFLAGS += -I$(KERNEL)/include -I$(IFX_PRODUCT_ROOT)/dect_ifx_catiq_stack -I$(IFX_VOIP_COMMON_DIR)/voip_ifx_common/h/ -I$(TAPI_DIR) -I$(src)/../dect_ifx_timer -I$(TAPI_DIR)/src/ -I$(IFX_TAPI_DIR)/src/lib/lib_bufferpool/

EXTRA_CFLAGS += $(IFX_CFLAGS) -DFT -DCONFIG_SWITCH_MDIO_ACC -DCONFIG_TWINPASS -DCATIQ_NOEMO $(VDSL2)#-DDEBUG #-gstabs+
EXTRA_CFLAGS += -DFT -DCATIQ -DDECT_NG -DDECT_NG_AS -DDECT_NG_WBS -DDECT_NG_FULL_PAGE -DUSE_VOICE_BUFFERPOOL
EXTRA_CFLAGS += -DVOICE_CYCLIC_INDEX_CHECK 
EXTRA_CFLAGS += -DCATIQ_UPLANE -DCOSIC_DMA -DUSE_PLC_BIT 
EXTRA_CFLAGS += $(BOARD)
#to enable debugs
#CFLAGS += -DIFX_COSIC_DEBUG
EXTRA_CFLAGS += -Wall -Wstrict-prototypes -Wno-trigraphs -O2 -fomit-frame-pointer -fno-strict-aliasing -fno-common -G 0 -mno-abicalls -fno-pic  -Wa,--trap -pipe -mlong-calls -DTAPI_LOOPBACK -DLINUX
cosic-objs := drv_dect.o fhmac.o fmbc.o drv_dect_cosic_drv.o fdebug.o
obj-m += cosic.o

clean:
	rm -f *.*o 
endif
