/*
 *
 * Author    Action     Date         Description
 * xiewenyi   New        2017/12/04   Created
 *
 * Copyright (C) bnxd
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <time.h> 
#include <sys/ioctl.h>
#include <linux/input.h>
#include <sys/ipc.h>  
#include <sys/shm.h> 
#include "driver.h" 
#include "app.h"
#include "test.h"


int app_usb_sda1_test(char *buf)
{
	int count = 0;
	if(bndriver_file_getsize("/tmp/sda1/autotestconfig.txt") <= 0)
	{
		system("mkdir /tmp/sda1");
		system("sudo chmod -R 777 /tmp/sda1");
		system("sudo chown linaro:linaro /tmp/sda1");
		system("ls /dev/sd* | grep \"/dev/sda1\" -c > /tmp/autotestsda1.txt");
		count = bndriver_file_context_num("/tmp/autotestsda1.txt");
		if(count < 1)
		{
			printf("\033[1;31;40m [fail] test usb sda1 fail\033[0m \n");
			sprintf(buf, "usb sda1 test fail--%d", count);
			return -1;
		}
		system("sudo mount -o umask=000 /dev/sda1 /tmp/sda1");		
	}
	if(bndriver_file_getsize("/tmp/sda1/autotestconfig.txt") <= 0)
	{
		printf("\033[1;31;40m [fail] test usb sda1 mount fail\033[0m \n");
		sprintf(buf, "usb sda1 mount test fail--%d", count);
		return -2;
	}
	if(file_rw_test("/tmp/sda1") < 0)
	{
		printf("\033[1;31;40m [fail] test usb sda1 rw fail\033[0m \n");;
		sprintf(buf, "usb sda1 rw test fail--%d", count);
		return -3;
	}	
	return 0;
}



int app_usb_sdb1_test(char *buf)
{
	int count = 0;
	if(bndriver_file_getsize("/tmp/sdb1/autotestconfig.txt") <= 0)
	{
		system("mkdir /tmp/sdb1");
		system("sudo chmod -R 777 /tmp/sdb1");
		system("sudo chown linaro:linaro /tmp/sdb1");
		system("ls /dev/sd* | grep \"/dev/sdb1\" -c > /tmp/autotestsdb1.txt");
		count = bndriver_file_context_num("/tmp/autotestsdb1.txt");
		if(count < 1)
		{
			printf("\033[1;31;40m [fail] test usb sdb1 fail\033[0m \n");
			sprintf(buf, "usb sdb1 test fail--%d", count);
			return -1;
		}
		system("sudo mount -o umask=000 /dev/sdb1 /tmp/sdb1");
	}
	if(bndriver_file_getsize("/tmp/sdb1/autotestconfig.txt") <= 0)
	{
		printf("\033[1;31;40m [fail] test usb sdb1 mount fail\033[0m \n");
		sprintf(buf, "usb sdb1 mount test fail--%d", count);
		return -2;
	}
	if(file_rw_test("/tmp/sdb1") < 0)
	{
		printf("\033[1;31;40m [fail] test usb sdb1 rw fail\033[0m \n");
		sprintf(buf, "usb sdb1 rw test fail--%d", count);
		return -3;
	}
	return 0;
}



int app_usb_sdc1_test(char *buf)
{
	int count = 0;
	if(bndriver_file_getsize("/tmp/sdc1/autotestconfig.txt") <= 0)
	{
		
		system("mkdir /tmp/sdc1");
		system("sudo chmod -R 777 /tmp/sdc1");
		system("sudo chown linaro:linaro /tmp/sdc1");
		printf("mkdir sdc1\n");
		system("ls /dev/sd* | grep \"/dev/sdc1\" -c > /tmp/autotestsdc1.txt");
		count = bndriver_file_context_num("/tmp/autotestsdc1.txt");
		if(count < 1)
		{
			printf("\033[1;31;40m [fail] test usb sdc1 fail\033[0m \n");
			sprintf(buf, "usb 2.0 test fail--%d", count);
			return -1;
		}
		system("sudo mount -o umask=000 /dev/sdc1 /tmp/sdc1");
	}
	if(bndriver_file_getsize("/tmp/sdc1/autotestconfig.txt") <= 0)
	{
		printf("\033[1;31;40m [fail] test usb sdc1 mount fail\033[0m \n");
		sprintf(buf, "usb sdc1 mount test fail--%d", count);
		return -2;
	}
	if(file_rw_test("/tmp/sdc1") < 0)
	{
		printf("\033[1;31;40m [fail] test usb sdc1 rw fail\033[0m \n");;
		sprintf(buf, "usb sdc1 rw test fail--%d", count);
		return -3;
	}
	return 0;
}

int app_usb_sda1_speed_test(void)
{
	char buf[256];
	system("echo \"***********************sda1 test**************************\r\n\" > /tmp/speedsda1.txt");
	system("echo \"sda1 speed test write 100M:\r\n\" >> /tmp/speedsda1.txt");
	if(app_usb_sda1_test(buf) < 0)
	{
		system("echo \"sda1 speed test write 100M: fail\r\n\" >> /tmp/speedsda1.txt");
		return -1;
	}
	system("cd /tmp/sda1");
	system("sudo dd if=/dev/zero of=./largefile bs=100M count=5 2>>/tmp/speedsda1.txt");
	system("echo \":\r\n\" >> /tmp/speedsda1.txt");
	system("echo \"sda1 speed test read 100M:\r\n\" >> /tmp/speedsda1.txt");
	system("sudo sh -c \"sync && echo 3 > /proc/sys/vm/drop_caches\"");
	system("sudo dd if=./largefile of=/dev/null bs=100M 2>>/tmp/speedsda1.txt");
	return 0;
}

int app_usb_sdb1_speed_test(void)
{
	char buf[256];
	system("echo \"***********************sdb1 test**************************\r\n\" > /tmp/speedsdb1.txt");
	system("echo \"sdb1 speed test write 100M:\r\n\" >> /tmp/speedsdb1.txt");
	if(app_usb_sdb1_test(buf) < 0)
	{
		system("echo \"sdb1 speed test write 100M: fail\r\n\" >> /tmp/speedsdb1.txt");
		return -1;
	}
	system("cd /tmp/sdb1");
	system("sudo dd if=/dev/zero of=./largefile bs=100M count=5 2>>/tmp/speedsdb1.txt");
	system("echo \":\r\n\" >> /tmp/speedsdb1.txt");
	system("echo \"sdb1 speed test read 100M:\r\n\" >> /tmp/speedsdb1.txt");
	system("sudo sh -c \"sync && echo 3 > /proc/sys/vm/drop_caches\"");
	system("sudo dd if=./largefile of=/dev/null bs=100M 2>>/tmp/speedsdb1.txt");
	return 0;
}

int app_usb_sdc1_speed_test(void)
{
	char buf[256];
	system("echo \"***********************sdc1 test**************************\r\n\" > /tmp/speedsdc1.txt");
	system("echo \"sdc1 speed test write 100M:\r\n\" >> /tmp/speedsdc1.txt");
	if(app_usb_sdc1_test(buf) < 0)
	{
		system("echo \"sdc1 speed test write 100M:fail\r\n\" >> /tmp/speedsdc1.txt");
		return -1;
	}
	system("cd /tmp/sdc1");
	system("sudo dd if=/dev/zero of=./largefile bs=100M count=5 2>>/tmp/speedsdc1.txt");
	system("echo \":\r\n\" >> /tmp/speedsdc1.txt");
	system("echo \"sdc1 speed test read 100M:\r\n\" >> /tmp/speedsdc1.txt");
	system("sudo sh -c \"sync && echo 3 > /proc/sys/vm/drop_caches\"");
	system("sudo dd if=./largefile of=/dev/null bs=100M 2>>/tmp/speedsdc1.txt");
	return 0;
}


