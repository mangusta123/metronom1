obj-m += use_hellomod3.o

all:
	make -C /path_to_linux_distro/linux_distro M=$(PWD) modules

clean:
	make -C /path_to_linux_distro/linux_distro M=$(PWD) clean