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

int app_tfcard_test_main(int flag)
{
	int count;
	char buf[256];

	system("df | grep \"/dev/mmcblk0p1\" -c > /tmp/tfmmcnode.txt");
	count = bndriver_file_context_num("/tmp/tfmmcnode.txt");
	if(count == 0)
	{
		return -1;
	}
	system("df | grep \"/mnt/sdcard\" -c > /tmp/tfmmcnodepath.txt");
	count = bndriver_file_context_num("/tmp/tfmmcnodepath.txt");
	if(count == 0)
	{
		return -1;
	}	
	count = bndriver_file_getsize("/mnt/sdcard/lianjiatestflag.txt");
	//printf("tf count =%d\n",count);
	if(count < 10)
	{
		return -1;
	}

	memset(buf, 0, sizeof(buf));
	bndriver_file_read("/mnt/sdcard/lianjiatestflag.txt", buf, 0, 10);
	if(0 == memcmp(buf, "1234567890", 10))
	{
		return 1;
	}
	return -1;
}


int app_tfcard_mount(int flag)
{
	int count;
	char buf[256];
	int i;
	
	for(i = 0; i < 10; i++)
	{
		system("sudo fdisk -l | grep \"/dev/mmcblk0p1\" -c > /tmp/tfmmcnode.txt");
		count = bndriver_file_context_num("/tmp/tfmmcnode.txt");
		//printf("mount sd ----%d\n",count);
		if(count != 0)
		{
			break;
		}
		bndriver_secondSleep(1);
	}
	if(i >= 10)
	{
		return -1;
	}
	
	//printf("mount sd 0\n");
	if(bndriver_file_getsize(PATH_AUTOTEST_CONFIG_FILE) <= 0)
	{
		//printf("mount sd\n");
		//system("rm /tmp/sdcard -rf");
		system("mkdir /tmp/sdcard");
		system("sudo chmod -R 777 /tmp/sdcard");
		system("sudo chown linaro:linaro /tmp/sdcard");
		//system("sudo umount /dev/mmcblk0p1");
		system("sudo mount -o umask=000 /dev/mmcblk0p1 /tmp/sdcard");

		bndriver_secondSleep(2);
		if(bndriver_file_getsize(PATH_AUTOTEST_CONFIG_FILE) <= 0)
		{
			return -1;
		}

		if(file_rw_test("/tmp/sdcard") < 0)
		{
			printf("\033[1;31;40m [fail] test tf card rw fail\033[0m \n");;
			return -1;
		}
	}
	return 1;
}


int app_tfcard_speed_test(void)
{
	system("echo \"***********************tf card speed test**************************\r\n\" > /tmp/speedtfcard.txt");
	system("cd /tmp/sdcard");
	system("echo \"tf card speed test write 100M:\r\n\" >> /tmp/speedtfcard.txt");
	system("sudo dd if=/dev/zero of=./largefile bs=100M count=5 2>>/tmp/speedtfcard.txt");
	system("echo \":\r\n\" >> /tmp/speedtfcard.txt");
	system("echo \"tf card speed test read 100M:\r\n\" >> /tmp/speedtfcard.txt");
	system("sudo sh -c \"sync && echo 3 > /proc/sys/vm/drop_caches\"");
	system("sudo dd if=./largefile of=/dev/null bs=100M 2>>/tmp/speedtfcard.txt");
	return 0;
}

