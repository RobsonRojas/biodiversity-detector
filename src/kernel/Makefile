# Makefile for Guardian Audio Module

KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) LLVM=1 modules

clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean
