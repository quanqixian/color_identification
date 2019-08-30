KERN_PATH:=/home/quan/kernel/linux-4.4.112
all:
	make -C $(KERN_PATH) M=$(shell pwd) modules
clean:
	make -C $(KERN_PATH) M=$(shell pwd) clean
	rm -rf a.out
obj-m:=demo.o

