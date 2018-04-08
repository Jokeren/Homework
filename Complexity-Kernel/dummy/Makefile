obj-m+=dummy.o
obj-m+=kdummy.o
obj-m+=ktest.o


all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean