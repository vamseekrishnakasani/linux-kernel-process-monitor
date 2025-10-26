obj-m += process_monitor.o

process_monitor-objs := src/process_monitor.o

KERNEL_DIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules

clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean

install:
	sudo insmod process_monitor.ko

uninstall:
	sudo rmmod process_monitor

logs:
	sudo dmesg | tail -50

watch:
	sudo dmesg -w | grep PROCESS_MONITOR