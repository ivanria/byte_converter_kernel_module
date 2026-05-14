obj-m			:= byte_converter.o
byte_converter-y	:= main.o proc.o conv_helpers.o
KERNEL_SRC		:= /lib/modules/$(shell uname -r)/build
PWD			:= $(shell pwd)
ccflags-y		:= -Wall -Wextra -Wmissing-prototypes \
			-Wstrict-prototypes -DBYTE_CONV_DEBUG
#ldflags-y		:= -T foo_section.lds

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules

install:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules_install

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) clean

help:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) help

