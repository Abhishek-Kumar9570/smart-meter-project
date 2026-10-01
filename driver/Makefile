obj-m += virtual_meter_driver.o

KDIR := /home/asus/WSL2-Linux-Kernel
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
