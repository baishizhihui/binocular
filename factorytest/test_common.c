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

char g_board_id[64];
char g_board_test_outfile[128];
char g_board_speed_outfile[128];
char g_config_service_ip[64];
char g_config_wifi_name[64];
char g_config_wifi_pwd[64];
char g_config_lan_ip[64];
char g_config_wifi_ip[64];
char g_config_wifi_gw[64];
char g_config_sn[64];


typedef struct _test_result_table {
	int result;
	char discribe[64];
} TEST_RESULT_TABLE;
TEST_RESULT_TABLE g_board_funtest[TEST_FUN_MAX];
int g_test_start = 0;

int file_rw_test(char *path)
{
	int i;
	int length;
	unsigned char buf[2048];
	unsigned char buf2[2048];
	char filename[256];

	for(i = 0; i < 2048; i++)
	{
		buf[i] = i%256;
	}
	memset(buf2, 0, sizeof(buf2));
	memset(filename, 0, sizeof(filename));
	strcpy(filename, path);
	strcat(filename, "/autotestfile.bin");
	printf("test filename=%s\n",filename);
	bndriver_file_new(filename);
	bndriver_file_add(filename, buf, 2048);
	length = bndriver_file_getsize(filename);
	if(length != 2048)
	{
		return -1;
	}
	bndriver_file_read(filename, buf2, 0, 2048);
	for(i = 0; i < 2048; i++)
	{
		if(buf[i] != buf2[i])
		{
			return -1;
		}
	}

	return 0;
}


int app_emmc_speed_test(void)
{
	system("echo \"***********************emmc test**************************\r\n\" > /tmp/speedemmc.txt");
	system("cd ~/");
	system("echo \"emmc speed test write 100M:\r\n\" >> /tmp/speedemmc.txt");
	system("dd if=/dev/zero of=./largefile bs=100M count=5 2>>/tmp/speedemmc.txt");
	system("echo \":\r\n\" >> /tmp/speedemmc.txt");
	system("echo \"emmc speed test read 100M:\r\n\" >> /tmp/speedemmc.txt");
	system("sudo sh -c \"sync && echo 3 > /proc/sys/vm/drop_caches\"");
	system("dd if=./largefile of=/dev/null bs=100M 2>>/tmp/speedemmc.txt");
}


int app_all_speed_result(void)
{
	int size;
	char buf[1024*16];
	char time_date[128];
	char time_time[128];

	memset(time_date, 0, sizeof(time_date));
	memset(time_time, 0, sizeof(time_time));
	bndriver_get_data_time(time_date, time_time);

	memset(buf, 0, sizeof(buf));
	sprintf(buf, "echo \"[%s %s]all speed test:\" >> %s", time_date, time_time, g_board_speed_outfile);	
	system(buf);

	//bndriver_file_new(g_board_speed_outfile);
	size = bndriver_file_getsize("/tmp/speedsda1.txt");
	if(size)
	{
		bndriver_file_read("/tmp/speedsda1.txt", buf, 0, size);
		bndriver_file_add(g_board_speed_outfile, buf, size);
	}
	size = bndriver_file_getsize("/tmp/speedsdb1.txt");
	if(size)
	{
		bndriver_file_read("/tmp/speedsdb1.txt", buf, 0, size);
		bndriver_file_add(g_board_speed_outfile, buf, size);
	}
	size = bndriver_file_getsize("/tmp/speedsdc1.txt");
	if(size)
	{
		bndriver_file_read("/tmp/speedsdc1.txt", buf, 0, size);
		bndriver_file_add(g_board_speed_outfile, buf, size);
	}
	size = bndriver_file_getsize("/tmp/speedlan.txt");
	if(size)
	{
		bndriver_file_read("/tmp/speedlan.txt", buf, 0, size);
		bndriver_file_add(g_board_speed_outfile, buf, size);
	}
	size = bndriver_file_getsize("/tmp/speedwifi.txt");
	if(size)
	{
		bndriver_file_read("/tmp/speedwifi.txt", buf, 0, size);
		bndriver_file_add(g_board_speed_outfile, buf, size);
	}	
	size = bndriver_file_getsize("/tmp/speedemmc.txt");
	if(size)
	{
		bndriver_file_read("/tmp/speedemmc.txt", buf, 0, size);
		bndriver_file_add(g_board_speed_outfile, buf, size);
	}
}

int board_function_test(int flag, char *buf)
{
	int i;
	int count = 0;
	char cmd_buf[128];
	char ret_buf[128];
	g_test_start = 1;
	switch(flag)
	{
		case TEST_FUN_WIFI:
			printf("test wifi\n");
			if(app_wifi_test(10) >= 0)
			{
				printf("test wifi success\n");
				g_board_funtest[flag].result = 1;
				strcpy(g_board_funtest[flag].discribe, "wifi test success");
				break;
			}
			printf("\033[1;31;40m [fail] test wifi\033[0m \n");
			g_board_funtest[flag].result = -1;
			strcpy(g_board_funtest[flag].discribe, "wifi test fail");
			break;

		case TEST_FUN_LAN:
			printf("test lan\n");
			if(app_lan_test(10) >= 0)
			{
				printf("test lan success\n");
				g_board_funtest[flag].result = 1;
				strcpy(g_board_funtest[flag].discribe, "lan test success");
				break;
			}

			printf("\033[1;31;40m [fail] test lan\033[0m \n");
			g_board_funtest[flag].result = -1;
			strcpy(g_board_funtest[flag].discribe, "lan test fail");
			break;
			
		case TEST_FUN_TF_CARD:
			g_board_funtest[flag].result = 1;
			strcpy(g_board_funtest[flag].discribe, "tf card test success");
#if 0
			if(app_tfcard_test_main(0) > 0)
			{
				printf("test tfcard success\n");
				g_board_funtest[flag].result = 1;
				strcpy(g_board_funtest[flag].discribe, "tf card test success");
			}
			else
			{
				printf("test tfcard fail\n");
				g_board_funtest[flag].result = -1;
				strcpy(g_board_funtest[flag].discribe, "tf card test fail");
			}
#endif			
			break;
			
		case TEST_FUN_USB:
			printf("test usb\n");
			system("lsusb -t | grep \"usb-storage, 5000M\" -c > /tmp/autotestusb30.txt");
			bndriver_secondSleep(1);
			count = bndriver_file_context_num("/tmp/autotestusb30.txt");
			if(count < 1)
			{
				printf("\033[1;31;40m [fail] test usb3.0 get num\033[0m \n");
				g_board_funtest[flag].result = -1;
				sprintf(g_board_funtest[flag].discribe, "usb 3.0 get num fail--%d", count);
				break;
			}

			system("lsusb -t | grep \"usb-storage, 480M\" -c > /tmp/autotestusb20.txt");
			bndriver_secondSleep(1);
			count = bndriver_file_context_num("/tmp/autotestusb20.txt");
			if(count < 2)
			{
				printf("\033[1;31;40m [fail] test usb2.0 get num\033[0m \n");
				g_board_funtest[flag].result = -1;
				sprintf(g_board_funtest[flag].discribe, "usb 2.0 get num fail--%d", count);
				break;
			}

			memset(ret_buf, 0, sizeof(ret_buf));
			if(app_usb_sda1_test(ret_buf) < 0)
			{
				g_board_funtest[flag].result = -1;
				sprintf(g_board_funtest[flag].discribe, "%s", ret_buf);
				break;
			}

			memset(ret_buf, 0, sizeof(ret_buf));
			if(app_usb_sdb1_test(ret_buf) < 0)
			{
				g_board_funtest[flag].result = -1;
				sprintf(g_board_funtest[flag].discribe, "%s", ret_buf);
				break;
			}

			memset(ret_buf, 0, sizeof(ret_buf));
			if(app_usb_sdc1_test(ret_buf) < 0)
			{
				g_board_funtest[flag].result = -1;
				sprintf(g_board_funtest[flag].discribe, "%s", ret_buf);
				break;
			}
			printf("test usb 3.0/2.0 success\n");
			g_board_funtest[flag].result = 1;
			strcpy(g_board_funtest[flag].discribe, "usb camera test success");
			break;

			
		case TEST_FUN_CAMERA:
			break;
			
		case TEST_FUN_AUDIO:
			break;
			
		case TEST_FUN_TYPEC:
			break;
			
		case TEST_FUN_HDMI:
			break;
			
		case TEST_FUN_UART:
			break;
			
		case TEST_FUN_MOTOR:
			break;
			
		case TEST_FUN_FAN:
			break;

		case TEST_FUN_IMU:
			if(imu_test_main() < 0)
			{
				printf("\033[1;31;40m [fail] test imu\033[0m \n");
				g_board_funtest[flag].result = -1;
				sprintf(g_board_funtest[flag].discribe, "%s", "imu test fail");
				break;
			}
			printf("test imu success\n");
			g_board_funtest[flag].result = 1;
			strcpy(g_board_funtest[flag].discribe, "imu test success");
			break;
			
		case TEST_FUN_ONOFF_KEY:
			break;		

		case TEST_FUN_WIFI_SPEED:
			if(app_wifi_speed_test() < 0)
			{
				printf("\033[1;31;40m [fail] test wifi speed\033[0m \n");
			}
			break;
		case TEST_FUN_LAN_SPEED:
			if(app_lan_speed_test() < 0)
			{
				printf("\033[1;31;40m [fail] test lan speed\033[0m \n");				
			}
			break;
		case TEST_FUN_USB_SPEED:
			if(app_usb_sda1_speed_test() < 0)
			{
				printf("\033[1;31;40m [fail] test sda1 speed\033[0m \n");
			}
			if(app_usb_sdb1_speed_test() < 0)
			{
				printf("\033[1;31;40m [fail] test sdb1 speed\033[0m \n");
			}
			if(app_usb_sdc1_speed_test() < 0)
			{
				printf("\033[1;31;40m [fail] test sdc1 speed\033[0m \n");
			}
			break;
		case TEST_FUN_TFCARD_SPEED:
			app_tfcard_speed_test();
			break;		

		case TEST_FUN_EMMC_SPEED:
			app_emmc_speed_test();
			break;	
	}
	g_test_start = 0;
	return 1;
}



int board_function_result(void)
{
	int offset;

	char buf[1024];
	char time_date[128];
	char time_time[128];

	memset(time_date, 0, sizeof(time_date));
	memset(time_time, 0, sizeof(time_time));
	bndriver_get_data_time(time_date, time_time);

	memset(buf, 0, sizeof(buf));
	sprintf(buf, "echo \"[%s %s]function test:\" >> %s", time_date, time_time, g_board_speed_outfile);	
	system(buf);

	offset = TEST_FUN_IMU;
	if(g_board_funtest[offset].result == 0)
	{
		printf(" [fail]  %s\n", "imu not test");
	}
	else if(g_board_funtest[offset].result < 0)
	{
		printf("\033[1;31;40m [fail] \033[0m %s\n", g_board_funtest[offset].discribe);
	}
	else if(g_board_funtest[offset].result > 0)
	{
		printf("\033[1;32;40m [pass] \033[0m %s\n", g_board_funtest[offset].discribe);
	}
	bndriver_file_add(g_board_test_outfile, g_board_funtest[offset].discribe, strlen(g_board_funtest[offset].discribe));
	bndriver_file_add(g_board_test_outfile, "\n", 1);

	offset = TEST_FUN_USB;
	if(g_board_funtest[offset].result == 0)
	{
		printf(" [fail]  %s\n", "usb not test");
	}
	else if(g_board_funtest[offset].result < 0)
	{
		printf("\033[1;31;40m [fail] \033[0m %s\n", g_board_funtest[offset].discribe);
	}
	else if(g_board_funtest[offset].result > 0)
	{
		printf("\033[1;32;40m [pass] \033[0m %s\n", g_board_funtest[offset].discribe);
	}
	bndriver_file_add(g_board_test_outfile, g_board_funtest[offset].discribe, strlen(g_board_funtest[offset].discribe));
	bndriver_file_add(g_board_test_outfile, "\n", 1);

	offset = TEST_FUN_WIFI;
	if(g_board_funtest[offset].result == 0)
	{
		printf(" [fail]  %s\n", "wifi not test");
	}
	else if(g_board_funtest[offset].result < 0)
	{
		printf("\033[1;31;40m [fail] \033[0m %s\n", g_board_funtest[offset].discribe);
	}
	else if(g_board_funtest[offset].result > 0)
	{
		printf("\033[1;32;40m [pass] \033[0m %s\n", g_board_funtest[offset].discribe);
	}
	bndriver_file_add(g_board_test_outfile, g_board_funtest[offset].discribe, strlen(g_board_funtest[offset].discribe));
	bndriver_file_add(g_board_test_outfile, "\n", 1);

	offset = TEST_FUN_LAN;
	if(g_board_funtest[offset].result == 0)
	{
		printf(" [fail]  %s\n", "lan not test");
	}
	else if(g_board_funtest[offset].result < 0)
	{
		printf("\033[1;31;40m [fail] \033[0m %s\n", g_board_funtest[offset].discribe);
	}
	else if(g_board_funtest[offset].result > 0)
	{
		printf("\033[1;32;40m [pass] \033[0m %s\n", g_board_funtest[offset].discribe);
	}
	bndriver_file_add(g_board_test_outfile, g_board_funtest[offset].discribe, strlen(g_board_funtest[offset].discribe));
	bndriver_file_add(g_board_test_outfile, "\n", 1);

	offset = TEST_FUN_TF_CARD;
	if(g_board_funtest[offset].result == 0)
	{
		printf(" [fail]  %s\n", "tf card not test");
	}
	else if(g_board_funtest[offset].result < 0)
	{
		printf("\033[1;31;40m [fail] \033[0m %s\n", g_board_funtest[offset].discribe);
	}
	else if(g_board_funtest[offset].result > 0)
	{
		printf("\033[1;32;40m [pass] \033[0m %s\n", g_board_funtest[offset].discribe);
	}
	bndriver_file_add(g_board_test_outfile, g_board_funtest[offset].discribe, strlen(g_board_funtest[offset].discribe));
	bndriver_file_add(g_board_test_outfile, "\n", 1);

	return 0;
}



int config_file_init(char *file)
{
	int len;
	int i;
	char buf[512];
	char *p = NULL;
	len = bndriver_file_getsize(file);
	if(len <= 0)
	{
		return -1;
	}
	memset(buf, 0, sizeof(buf));
	if(bndriver_file_read(file, buf, 0, len) <= 0)
	{
		return -1;
	}

	p = strstr(buf, "serviceip");
	if(NULL == p)
	{
		return -1;
	}
	p += 10;
	for(i = 0; i < 50 ; i++)
	{
		if((*(p+i) == 0x0d)||(*(p+i) == 0x0a))
		{
			break;
		}
		g_config_service_ip[i] = *(p+i);
	}

	p = strstr(buf, "wifiname");
	if(NULL == p)
	{
		return -1;
	}
	p += 9;
	for(i = 0; i < 50 ; i++)
	{
		if((*(p+i) == 0x0d)||(*(p+i) == 0x0a))
		{
			break;
		}
		g_config_wifi_name[i] = *(p+i);
	}

	p = strstr(buf, "wifipwd");
	if(NULL == p)
	{
		return -1;
	}
	p += 8;
	for(i = 0; i < 50 ; i++)
	{
		if((*(p+i) == 0x0d)||(*(p+i) == 0x0a))
		{
			break;
		}
		g_config_wifi_pwd[i] = *(p+i);
	}
	
	p = strstr(buf, "lanip");
	if(NULL == p)
	{
		return -1;
	}
	p += 6;
	for(i = 0; i < 50 ; i++)
	{
		if((*(p+i) == 0x0d)||(*(p+i) == 0x0a))
		{
			break;
		}
		g_config_lan_ip[i] = *(p+i);
	}

	p = strstr(buf, "wifiip");
	if(NULL == p)
	{
		return -1;
	}
	p += 7;
	for(i = 0; i < 50 ; i++)
	{
		if((*(p+i) == 0x0d)||(*(p+i) == 0x0a))
		{
			break;
		}
		g_config_wifi_ip[i] = *(p+i);
	}

	p = strstr(buf, "wifigw");
	if(NULL == p)
	{
		return -1;
	}
	p += 7;
	for(i = 0; i < 50 ; i++)
	{
		if((*(p+i) == 0x0d)||(*(p+i) == 0x0a))
		{
			break;
		}
		g_config_wifi_gw[i] = *(p+i);
	}

	p = strstr(buf, "bdsn");
	if(NULL == p)
	{
		return -1;
	}
	p += 5;
	for(i = 0; i < 50 ; i++)
	{
		if((*(p+i) == 0x0d)||(*(p+i) == 0x0a))
		{
			break;
		}
		g_config_sn[i] = *(p+i);
	}


}


void sigroutine(int signo){
	int ret;
	switch (signo){
		case SIGALRM:
			//printf("timer\n");
			auto_test_timer_callback();
			signal(SIGALRM, sigroutine);
			break;

		case SIGVTALRM:
			printf("Catch a signal -- SIGVTALRM \n");
			signal(SIGVTALRM, sigroutine);
			break;
	}
	return;
}

void timer_init(int time)
{
	struct itimerval value, ovalue, value2;          //(1)
	//printf("process id is %d/n", getpid());
	signal(SIGALRM, sigroutine);
	//signal(SIGVTALRM, sigroutine);
	value.it_value.tv_sec = 1;
	value.it_value.tv_usec = 0;
	value.it_interval.tv_sec = 1;
	value.it_interval.tv_usec = 0;
	setitimer(ITIMER_REAL, &value, &ovalue);    //(2)

	//g_SENSOR_DATA.maxcount = time/100;
	/*value2.it_value.tv_sec = 0;
	value2.it_value.tv_usec = 500000;
	value2.it_interval.tv_sec = 0;
	value2.it_interval.tv_usec = 500000;
	setitimer(ITIMER_VIRTUAL, &value2, &ovalue);*/
}

void auto_test_init(void)
{
	memset(g_board_funtest, 0, sizeof(g_board_funtest));
	timer_init(5000);
}


