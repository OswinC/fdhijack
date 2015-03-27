CROSS_COMPILE = arm-linux-
CC = $(CROSS_COMPILE)gcc

LINUX_DEBUG = false

KDIR := $(KERNELROOT)
PWD := $(shell pwd)

export CC

obj-m := fdhj.o
MODULE = fdhj.ko

CFLAGS_fdhj.o += -I$(PWD)/../inc
export CFLAGS_fdhj.o

default:
	$(MAKE) ARCH=arm CROSS_COMPILE=arm-linux- -C $(KERNELSRC) O=$(PRODUCTDIR)/tmp/kernel M=$(PWD) modules
clean:
	rm -rf *.ko *.o *.mod.* *.symvers *.c~ .mod* .fdhj* .test* .tmp* modules.order
cleantmp:
	rm -rf *.o *.mod.* *.symvers *.c~ .mod* .fdhj* .test* .tmp* modules.order

