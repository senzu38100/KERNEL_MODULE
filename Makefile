object := sct5
obj-m := $(object).o
pwd := $(CURDIR)
moddir := /lib/modules/6.1.0-29-amd64/build

.PHONY: clean

all: build sign
sign:
	/usr/src/linux/scripts/sign-file sha256 MOK.priv MOK.der $(object).ko

build:
	make -C $(moddir) M=$(pwd) modules

clean:
	make -C $(moddir) M=$(pwd) clean 


