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


int g_wifi_set_flag = 0;
extern char g_config_service_ip[64];
extern char g_config_wifi_name[64];
extern char g_config_wifi_pwd[64];
extern char g_config_lan_ip[64];
extern char g_config_wifi_ip[64];
extern char g_config_wifi_gw[64];

int app_lan_test(int pingcount)
{
	int i;
	int count = 0;
	char cmd_buf[128];
	system("sudo ifconfig wlan0 down");
	system("sudo ifconfig eth0 down");

	system("sudo ifconfig eth0 up");
	bndriver_secondSleep(8);

	for(i = 0; i < 5; i++)
	{
		if(bndriver_check_lan() >= 0)
		{
			break;
		}
	}
	if(i < 5)
	{
		bndriver_secondSleep(2);
		printf("satrt eth0 ping %s\n", g_config_service_ip);
		memset(cmd_buf, 0, sizeof(cmd_buf));
		//system("ping -c 10 192.168.0.104 | grep ttl=64 -c > /tmp/lanpingcount.txt");
		sprintf(cmd_buf, "sudo ping -c %d %s | grep icmp_seq -c > /tmp/lanpingcount.txt", pingcount, g_config_service_ip);
		system(cmd_buf);
		count = bndriver_file_context_num("/tmp/lanpingcount.txt");
		if(count >= pingcount)
		{
			return 0;
		}
	}

	return -1;
}

int app_wifi_test(int pingcount)
{
	int i;
	int count = 0;
	char cmd_buf[128];
	memset(cmd_buf, 0, sizeof(cmd_buf));
	system("sudo ifconfig wlan0 down");
	system("sudo ifconfig eth0 down");
	system("sudo cp /tmp/sdcard/interfaces /etc/network/");

	if(g_wifi_set_flag == 0)
	{
		g_wifi_set_flag = 1;
		system("sudo ifup wlan0");
		bndriver_secondSleep(1);
	}
	system("sudo ifconfig wlan0 up");
	if(bndriver_check_wlan() >= 0)
	{
		bndriver_secondSleep(2);
		printf("satrt wlan0 ping %s\n", g_config_service_ip);
		memset(cmd_buf, 0, sizeof(cmd_buf));
		//system("ping -c 10 192.168.0.104 | grep ttl=64 -c > /tmp/wifipingcount.txt");
		sprintf(cmd_buf, "sudo ping -c %d %s | grep icmp_seq -c > /tmp/wifipingcount.txt", pingcount, g_config_service_ip);
		system(cmd_buf);

		count = bndriver_file_context_num("/tmp/wifipingcount.txt");
		if(count >= pingcount)
		{
			return 0;
		}
	}
	return -1;
}

int app_lan_speed_test(void)
{
	char cmd_buf[128];
	system("echo \"[BSZH_Module_Result]lan speed test**************************\r\n\" > /tmp/speedlan.txt");
	if(app_lan_test(2) < 0)
	{
		system("echo \"lan speed fail\r\n\" >> /tmp/speedlan.txt");
		printf("\033[1;31;40m [fail] test lan speed fail\033[0m \n");
		return -1;
	}
	memset(cmd_buf, 0, sizeof(cmd_buf));
	printf("satrt test lan iperf, wait...\n");
	sprintf(cmd_buf, "iperf -c %s -d -t 60 >> /tmp/speedlan.txt", g_config_service_ip);
	system(cmd_buf);

	return 0;
}

int app_wifi_speed_test(void)
{
	char cmd_buf[128];
	system("echo \"[BSZH_Module_Result]wifi speed test*************************\r\n\" > /tmp/speedwifi.txt");
	if(app_wifi_test(2) < 0)
	{
		system("echo \"wifi speed fail\r\n\" >> /tmp/speedwifi.txt");
		printf("\033[1;31;40m [fail] test wifi speed fail\033[0m \n");
		return -1;
	}
	memset(cmd_buf, 0, sizeof(cmd_buf));
	printf("satrt test wifi iperf, wait...\n");
	sprintf(cmd_buf, "iperf -c %s -d -t 60 >> /tmp/speedwifi.txt", g_config_service_ip);
	system(cmd_buf);

	return 0;
}

int g_ntpdate_ok = 0;
int app_lan_update_time(void)
{
	char cmd[1024];  
	char data[1024];  
	char *presult;  
	int time = 0;

	if(g_ntpdate_ok)
	{
		return 0;
	}

	app_lan_test(2);
	system("rm -rf /tmp/ntp.ok");
	while(1)
	{
		sprintf(cmd,"sudo ntpdate -u %s",g_config_service_ip); 
		printf("%s\n", cmd);
		memset(data, 0, sizeof(data));
		bndriver_super_system(cmd, data, sizeof(data)); 
		system(cmd);
		printf("send %s\n", cmd);

		presult = strstr(data, "adjust time server");  
		if (presult != NULL)
		{
			system("echo 1 > /tmp/ntp.ok");
			system("sudo hwclock --systohc"); 
			system("sudo hwclock -w"); 
			printf("adjust time server ok\n");
			g_ntpdate_ok = 1;
			return 1;
		}
		presult = strstr(data,"step time server");  
		if (presult != NULL) 
		{
			system("echo 1 > /tmp/ntp.ok");
			system("sudo hwclock --systohc"); 
			system("sudo hwclock -w"); 
			printf("step time server ok\n");
			g_ntpdate_ok = 1;
			return 1; 
		}
		sleep(5);
		time++;
		if(time > 24)
		{
			return -1;
		}
	}
	return 0;
}



