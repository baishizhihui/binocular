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

int app_usb_mount_dev(char *point)
{
	int i;
	char cmd_buf[128];
	int templen;
	int ret;
	char tempbuf[128];
	char *p;
	char *psearch;
	memset(cmd_buf, 0, sizeof(cmd_buf));
	sprintf(cmd_buf, "ls %s* > /tmp/usbmount.txt", point);
	system(cmd_buf);	

	templen = bndriver_file_getsize("/tmp/usbmount.txt");
	if(templen == 0)
	{
		return 0;
	}
	memset(tempbuf, 0, sizeof(tempbuf));
	bndriver_file_read("/tmp/usbmount.txt", tempbuf, 0, templen);
	psearch = tempbuf;

	for(i = 0; i < 10; i++)
	{
		p = strstr(psearch, point);
		if(p == NULL)
		{
			return 0;
		}	
		//printf("find %s,%d\n",p,strlen(point));
		p += strlen(point);
		if((*p <= '9')&&(*p >= '1'))
		{
			return (*p);
		}
		psearch = p;	
	}

	return 0;
}




int app_usb_sda1_test(char *buf)
{
	int count = 0;
	char num = 0;
	char cmd_buf[128];
	if(bndriver_file_getsize("/tmp/sda1/autotestconfig.txt") <= 0)
	{
		system("mkdir /tmp/sda1");
		system("sudo chmod -R 777 /tmp/sda1");
		system("sudo chown linaro:linaro /tmp/sda1");
		num = app_usb_mount_dev("/dev/sda");
		printf("even find /dev/sda%d\n", num-'0');
		if(0 == num)
		{
			printf("\033[1;31;40m [fail] test usb sda fail\033[0m \n");
			sprintf(buf, "usb sda test fail--%d", num-'0');
			return -1;
		}
		memset(cmd_buf, 0, sizeof(cmd_buf));
		sprintf(cmd_buf, "sudo umount /dev/sda%d", num-'0');
		system(cmd_buf);

		memset(cmd_buf, 0, sizeof(cmd_buf));
		sprintf(cmd_buf, "sudo mount -o umask=000 /dev/sda%d /tmp/sda1", num-'0');
		system(cmd_buf);		
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
	char num = 0;
	char cmd_buf[128];
	if(bndriver_file_getsize("/tmp/sdb1/autotestconfig.txt") <= 0)
	{
		system("mkdir /tmp/sdb1");
		system("sudo chmod -R 777 /tmp/sdb1");
		system("sudo chown linaro:linaro /tmp/sdb1");
		num = app_usb_mount_dev("/dev/sdb");
		printf("even find /dev/sdb%d\n", num-'0');
		if(0 == num)
		{
			printf("\033[1;31;40m [fail] test usb sdb fail\033[0m \n");
			sprintf(buf, "usb sdb test fail--%d", num-'0');
			return -1;
		}
		memset(cmd_buf, 0, sizeof(cmd_buf));
		sprintf(cmd_buf, "sudo umount /dev/sdb%d", num-'0');
		system(cmd_buf);

		memset(cmd_buf, 0, sizeof(cmd_buf));
		sprintf(cmd_buf, "sudo mount -o umask=000 /dev/sdb%d /tmp/sdb1", num-'0');
		system(cmd_buf);
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
	char num = 0;
	char cmd_buf[128];
	if(bndriver_file_getsize("/tmp/sdc1/autotestconfig.txt") <= 0)
	{
		
		system("mkdir /tmp/sdc1");
		system("sudo chmod -R 777 /tmp/sdc1");
		system("sudo chown linaro:linaro /tmp/sdc1");
		num = app_usb_mount_dev("/dev/sdc");
		printf("even find /dev/sdc%d\n", num-'0');
		if(0 == num)
		{
			printf("\033[1;31;40m [fail] test usb sdc fail\033[0m \n");
			sprintf(buf, "usb sdc test fail--%d", num-'0');
			return -1;
		}
		memset(cmd_buf, 0, sizeof(cmd_buf));
		sprintf(cmd_buf, "sudo umount /dev/sdc%d", num-'0');
		system(cmd_buf);

		memset(cmd_buf, 0, sizeof(cmd_buf));
		sprintf(cmd_buf, "sudo mount -o umask=000 /dev/sdc%d /tmp/sdc1", num-'0');
		system(cmd_buf);
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

int app_usb_sda1_speed_test(int flag)
{
	char buf[256];
	printf("***********************usb1 speed test**************************\r\n");
	if(flag)
	{
		if(app_usb_sda1_test(buf) < 0)
		{
			printf("\033[1;31;40m [fail] sda1 speed test fail\033[0m \n");
			return -1;
		}
		app_dd_speed_test("/tmp/sda1", NULL);
	}
	else 
	{
		system("echo \"[BSZH_Module_Result]usb1 test**************************\r\n\" > /tmp/speedsda1.txt");
		if(app_usb_sda1_test(buf) < 0)
		{
			system("echo \"sda1 speed test write 100M: fail\r\n\" >> /tmp/speedsda1.txt");
			printf("\033[1;31;40m [fail] sda1 speed test fail\033[0m \n");
			return -1;
		}
		app_dd_speed_test("/tmp/sda1", "/tmp/speedsda1.txt");
	}
	return 0;
}

int app_usb_sdb1_speed_test(int flag)
{
	char buf[256];
	printf("***********************usb2 speed test**************************\r\n");
	if(flag)
	{
		if(app_usb_sdb1_test(buf) < 0)
		{
			printf("\033[1;31;40m [fail] sdb1 speed test fail\033[0m \n");
			return -1;
		}
		app_dd_speed_test("/tmp/sdb1", NULL);
	}
	else 
	{
		system("echo \"[BSZH_Module_Result]usb2 test**************************\r\n\" > /tmp/speedsdb1.txt");
		if(app_usb_sdb1_test(buf) < 0)
		{
			system("echo \"sdb1 speed test write 100M: fail\r\n\" >> /tmp/speedsdb1.txt");
			printf("\033[1;31;40m [fail] sdb1 speed test fail\033[0m \n");
			return -1;
		}
		app_dd_speed_test("/tmp/sdb1", "/tmp/speedsdb1.txt");
	}
	return 0;
}

int app_usb_sdc1_speed_test(int flag)
{
	char buf[256];
	printf("***********************usb3 speed test**************************\r\n");
	if(flag)
	{
		if(app_usb_sdc1_test(buf) < 0)
		{
			printf("\033[1;31;40m [fail] sdc1 speed test fail\033[0m \n");
			return -1;
		}
		app_dd_speed_test("/tmp/sdc1", NULL);
	}
	else 
	{
		system("echo \"[BSZH_Module_Result]usb3 test**************************\r\n\" > /tmp/speedsdc1.txt");
		if(app_usb_sdc1_test(buf) < 0)
		{
			system("echo \"sdc1 speed test write 100M:fail\r\n\" >> /tmp/speedsdc1.txt");
			printf("\033[1;31;40m [fail] sdc1 speed test fail\033[0m \n");
			return -1;
		}
		app_dd_speed_test("/tmp/sdc1", "/tmp/speedsdc1.txt");
	}
	return 0;
}


