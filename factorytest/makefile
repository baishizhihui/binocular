INCLUDES = -I ./include
LIB= -L ./lib

ARCH=arm
CROSS_COMPILE=aarch64-linux-gnu-
#CROSS_COMPILE=arm-linux-gnueabihf-
CC=$(CROSS_COMPILE)gcc
#LD=$(CROSS_COMPILE)ld
#AR=$(CROSS_COMPILE)ar
#OBJCOPY=$(CROSS_COMPILE)objcopy


SOURCE_FILE += test_baidu.c
SOURCE_FILE += test_common.c
SOURCE_FILE += app_imu.c
SOURCE_FILE += app_tfcard.c
SOURCE_FILE += app_gpio.c
SOURCE_FILE += app_usb.c
SOURCE_FILE += app_net.c
SOURCE_FILE += main.c

SOURCE_FILE2 += test_baidu_burnin.c
SOURCE_FILE2 += test_common.c
SOURCE_FILE2 += app_imu.c
SOURCE_FILE2 += app_tfcard.c
SOURCE_FILE2 += app_gpio.c
SOURCE_FILE2 += app_usb.c
SOURCE_FILE2 += app_net.c
SOURCE_FILE2 += main.c


all:
	$(CC) $(SOURCE_FILE) -o bin/boteyeonetest $(INCLUDES) $(LIB) -l bsdriver

burnintest:
	$(CC) $(SOURCE_FILE2) -o bin/boteyeoneburnintest $(INCLUDES) $(LIB) -l bsdriver	

.PHONLY: clean

clean: 
	@-rm *.o *.a
