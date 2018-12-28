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
	//printf("test filename=%s\n",filename);
	bndriver_file_new(filename);
	bndriver_file_add(filename, buf, 2048);
	length = bndriver_file_getsize(filename);
	if(length != 2048)
	{
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "sudo rm %s", filename);	
		system(buf);
		return -1;
	}
	bndriver_file_read(filename, buf2, 0, 2048);
	for(i = 0; i < 2048; i++)
	{
		if(buf[i] != buf2[i])
		{
			memset(buf, 0, sizeof(buf));
			sprintf(buf, "sudo rm %s", filename);	
			system(buf);
			return -1;
		}
	}
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "sudo rm %s", filename);	
	system(buf);
	return 0;
}


int net_file_rw_test(void)
{
	int i,j;
	int length;
	unsigned char buf[2048];
	unsigned char buf2[2048];
	char filename[256];

	for(i = 0; i < 2048; i++)
	{
		buf[i] = i%256;
	}
	memset(filename, 0, sizeof(filename));
	strcpy(filename, "/home/linaro/auto_self_diagnosis/testfile.bin");
	//printf("test 00 filename=%s\n",filename);
	length = bndriver_file_getsize(filename);
	if(length != (1024*1024*512))
	{
		printf("big file length error-%d\n",length);
		return -1;
	}
	
	for(j = 0; j < 1024*256; j++)
	{
		memset(buf2, 0, sizeof(buf2));
		bndriver_file_read(filename, buf2, j*2048, 2048);
		for(i = 0; i < 2048; i++)
		{
			if(buf[i] != buf2[i])
			{
				printf("big file length == error %d\n", j);
				return -1;
			}
		}
	}
	return 0;
}


int net_file_new(void)
{
	int i,j;
	unsigned char buf[2048];
	char filename[256];

	for(i = 0; i < 2048; i++)
	{
		buf[i] = i%256;
	};
	memset(filename, 0, sizeof(filename));
	strcpy(filename, "testfile.bin");
	//printf("test filename=%s\n",filename);

	bndriver_file_new(filename);
	for(j = 0; j < 256*1024; j++)
	{
		bndriver_file_add(filename, buf, 2048);
	}

	return 0;
}

int app_dd_speed_test(char *path, char *outputfile)
{
	char buf[1024];

	memset(buf, 0, sizeof(buf));
	sprintf(buf, "cd %s", path);	
	printf("%s\n", buf);
	system(buf);

	if(outputfile == NULL)
	{
		printf("speed test write:\r\n");
		//system("sudo time dd if=/dev/zero of=./largefile bs=64k count=16k conv=fdatasync");// oflag=dsync
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "sudo dd if=/dev/zero of=%s/largefile bs=64k count=4k conv=fdatasync", path);	
		system(buf);
		printf(" \r\n");
		printf("speed test read:\r\n");
		system("sudo sh -c \"sync && echo 3 > /proc/sys/vm/drop_caches\"");
		//system("sudo dd if=./largefile of=/dev/null bs=64k");
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "sudo dd if=%s/largefile of=/dev/null bs=64k", path);	
		system(buf);
	}
	else
	{
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "echo \"speed test write:\r\n\" >>%s", outputfile);	
		system(buf);

		memset(buf, 0, sizeof(buf));
		//system("sudo dd if=/dev/zero of=./largefile bs=64k count=4k");// oflag=dsync
		sprintf(buf, "sudo dd if=/dev/zero of=%s/largefile bs=64k count=4k conv=fdatasync 2>>%s", path, outputfile);
		system(buf);

		memset(buf, 0, sizeof(buf));
		sprintf(buf, "echo \"speed test read:\r\n\" >>%s", outputfile);	
		system(buf);

		system("sudo sh -c \"sync && echo 3 > /proc/sys/vm/drop_caches\"");
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "sudo dd if=%s/largefile of=/dev/null bs=64k 2>>%s", path, outputfile);
		system(buf);
	}
	return 0;
}


int app_emmc_speed_test(int flag)
{
	printf("***********************emmc speed test**************************\r\n");
	if(flag)
	{
		app_dd_speed_test("/home/linaro/", NULL);
	}
	else 
	{
		system("echo \"[BSZH_Module_Result]emmc test**************************\r\n\" > /tmp/speedemmc.txt");
		app_dd_speed_test("/home/linaro/", "/tmp/speedemmc.txt");
	}
}


int app_all_speed_result(void)
{
	int size;
	char buf[1024*16];
	char outputfile[128];
	char time_date[128];
	char time_time[128];
	double a, b;

	memset(time_date, 0, sizeof(time_date));
	memset(time_time, 0, sizeof(time_time));
	bndriver_get_data_time(time_date, time_time);

	memset(outputfile, 0, sizeof(outputfile));
	sprintf(outputfile, "/home/linaro/boteyeone_speed_%s_%s.txt", time_date, time_time);
	bndriver_file_new(outputfile);	
	
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "echo \"[BSZH_Result][%s %s]all speed test:\" >> %s", time_date, time_time, g_board_speed_outfile);	
	system(buf);

	//bndriver_file_new(g_board_speed_outfile);
	size = bndriver_file_getsize("/tmp/speedsda1.txt");
	if(size)
	{
		memset(buf, 0, sizeof(buf));
		bndriver_file_read("/tmp/speedsda1.txt", buf, 0, size);
		bndriver_file_add(g_board_speed_outfile, buf, size);
		bndriver_file_add(outputfile, buf, size);
		bndriver_get_dd_speed_num(buf, size, &a, &b);
		printf("usb1 speed : write-%.2lf MB/s, read-%.2lf MB/s\n", a, b);
		if((!a) || (!b))
		{
			printf("\033[1;31;40m [fail] test usb1 speed\033[0m \n");
		}
	}

	size = bndriver_file_getsize("/tmp/speedsdb1.txt");
	if(size)
	{
		memset(buf, 0, sizeof(buf));
		bndriver_file_read("/tmp/speedsdb1.txt", buf, 0, size);
		bndriver_file_add(g_board_speed_outfile, buf, size);
		bndriver_file_add(outputfile, buf, size);
		bndriver_get_dd_speed_num(buf, size, &a, &b);
		printf("usb2 speed : write-%.2lf MB/s, read-%.2lf MB/s\n", a, b);
		if((!a) || (!b))
		{
			printf("\033[1;31;40m [fail] test usb2 speed\033[0m \n");
		}
	}
	size = bndriver_file_getsize("/tmp/speedsdc1.txt");
	if(size)
	{
		memset(buf, 0, sizeof(buf));
		bndriver_file_read("/tmp/speedsdc1.txt", buf, 0, size);
		bndriver_file_add(g_board_speed_outfile, buf, size);
		bndriver_file_add(outputfile, buf, size);
		bndriver_get_dd_speed_num(buf, size, &a, &b);
		printf("usb3 speed : write-%.2lf MB/s, read-%.2lf MB/s\n", a, b);
		if((!a) || (!b))
		{
			printf("\033[1;31;40m [fail] test usb3 speed\033[0m \n");
		}
	}
	size = bndriver_file_getsize("/tmp/speedemmc.txt");
	if(size)
	{
		memset(buf, 0, sizeof(buf));
		bndriver_file_read("/tmp/speedemmc.txt", buf, 0, size);
		bndriver_file_add(g_board_speed_outfile, buf, size);
		bndriver_file_add(outputfile, buf, size);
		bndriver_get_dd_speed_num(buf, size, &a, &b);
		printf("emmc speed : write-%.2lf MB/s, read-%.2lf MB/s\n", a, b);
		if((!a) || (!b))
		{
			printf("\033[1;31;40m [fail] test emmc speed\033[0m \n");
		}
	}
	size = bndriver_file_getsize("/tmp/speedtfcard.txt");
	if(size)
	{
		memset(buf, 0, sizeof(buf));
		bndriver_file_read("/tmp/speedtfcard.txt", buf, 0, size);
		bndriver_file_add(g_board_speed_outfile, buf, size);
		bndriver_file_add(outputfile, buf, size);
		bndriver_get_dd_speed_num(buf, size, &a, &b);
		printf("tfcard speed : write-%.2lf MB/s, read-%.2lf MB/s\n", a, b);
		if((!a) || (!b))
		{
			printf("\033[1;31;40m [fail] test tfcard speed\033[0m \n");
		}
	}
	size = bndriver_file_getsize("/tmp/speedlan.txt");
	if(size)
	{
		memset(buf, 0, sizeof(buf));
		bndriver_file_read("/tmp/speedlan.txt", buf, 0, size);
		bndriver_file_add(g_board_speed_outfile, buf, size);
		bndriver_file_add(outputfile, buf, size);
		bndriver_get_iperf_speed_num(buf, size, &a, &b);
		printf("lan speed : send-%.2lf Mbits/sec, recv-%.2lf Mbits/sec\n", a, b);
		if((a < 800) || (b < 800))
		{
			printf("\033[1;31;40m [fail] test lan speed\033[0m \n");
		}
	}
	size = bndriver_file_getsize("/tmp/speedwifi.txt");
	if(size)
	{
		memset(buf, 0, sizeof(buf));
		bndriver_file_read("/tmp/speedwifi.txt", buf, 0, size);
		bndriver_file_add(g_board_speed_outfile, buf, size);
		bndriver_file_add(outputfile, buf, size);
		bndriver_get_iperf_speed_num(buf, size, &a, &b);
		printf("wifi speed : send-%.2lf Mbits/sec, recv-%.2lf Mbits/sec\n", a, b);
		if((!a) || (!b))
		{
			printf("\033[1;31;40m [fail] test wifi speed\033[0m \n");
		}
	}	

}

int board_function_test(int flag, char *buf)
{
	int i;
	int count = 0;
	int count2 = 0;
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
				if(net_file_rw_test() < 0)
				{
					printf("\033[1;31;40m [fail] test lan big file error\033[0m \n");
					g_board_funtest[flag].result = -1;
					strcpy(g_board_funtest[flag].discribe, "lan rw big file fail");
					break;
				}
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
			//g_board_funtest[flag].result = 1;
			//strcpy(g_board_funtest[flag].discribe, "tf card test success");
#if 1
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

			system("lsusb -t | grep \"usb-storage, 480M\" -c > /tmp/autotestusb20.txt");
			bndriver_secondSleep(1);
			count2 = bndriver_file_context_num("/tmp/autotestusb20.txt");
			if((count < 1)||(count2 < 2))
			{
				printf("\033[1;31;40m [fail] test usb get num 3.0[%d], 2.0[%d]\033[0m \n",count, count2);
				g_board_funtest[flag].result = -1;
				sprintf(g_board_funtest[flag].discribe, "usb 3.0 get num=%d, usb 2.0 get num=%d", count, count2);
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
			if(app_usb_sda1_speed_test(0) < 0)
			{
				printf("\033[1;31;40m [fail] test sda1 speed\033[0m \n");
			}
			if(app_usb_sdb1_speed_test(0) < 0)
			{
				printf("\033[1;31;40m [fail] test sdb1 speed\033[0m \n");
			}
			if(app_usb_sdc1_speed_test(0) < 0)
			{
				printf("\033[1;31;40m [fail] test sdc1 speed\033[0m \n");
			}
			break;
		case TEST_FUN_TFCARD_SPEED:
			app_tfcard_speed_test(0);
			break;		

		case TEST_FUN_EMMC_SPEED:
			app_emmc_speed_test(0);
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
	char outputfile[128];

	memset(time_date, 0, sizeof(time_date));
	memset(time_time, 0, sizeof(time_time));
	bndriver_get_data_time(time_date, time_time);

	memset(buf, 0, sizeof(buf));
	sprintf(buf, "echo \"[BSZH_Result][%s %s]function test:\" >> %s", time_date, time_time, g_board_speed_outfile);	
	system(buf);

	memset(outputfile, 0, sizeof(outputfile));
	sprintf(outputfile, "/home/linaro/boteyeone_test_%s_%s.txt", time_date, time_time);
	bndriver_file_new(outputfile);

#ifdef ADD_IMU_TEST
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
	bndriver_file_add(outputfile, g_board_funtest[offset].discribe, strlen(g_board_funtest[offset].discribe));
	bndriver_file_add(outputfile, "\n", 1);
#endif

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
	bndriver_file_add(outputfile, g_board_funtest[offset].discribe, strlen(g_board_funtest[offset].discribe));
	bndriver_file_add(outputfile, "\n", 1);

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
	bndriver_file_add(outputfile, g_board_funtest[offset].discribe, strlen(g_board_funtest[offset].discribe));
	bndriver_file_add(outputfile, "\n", 1);

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
	bndriver_file_add(outputfile, g_board_funtest[offset].discribe, strlen(g_board_funtest[offset].discribe));
	bndriver_file_add(outputfile, "\n", 1);

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
	bndriver_file_add(outputfile, g_board_funtest[offset].discribe, strlen(g_board_funtest[offset].discribe));
	bndriver_file_add(outputfile, "\n", 1);
	return 0;
}


int cpu_freq_init(void)
{
	system("sudo chown linaro:linaro /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor");
	system("sudo echo performance > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor");

	system("sudo chown linaro:linaro /sys/devices/system/cpu/cpu1/cpufreq/scaling_governor");
	system("sudo echo performance > /sys/devices/system/cpu/cpu1/cpufreq/scaling_governor");

	system("sudo chown linaro:linaro /sys/devices/system/cpu/cpu2/cpufreq/scaling_governor");
	system("sudo echo performance > /sys/devices/system/cpu/cpu2/cpufreq/scaling_governor");

	system("sudo chown linaro:linaro /sys/devices/system/cpu/cpu3/cpufreq/scaling_governor");
	system("sudo echo performance > /sys/devices/system/cpu/cpu3/cpufreq/scaling_governor");

	system("sudo chown linaro:linaro /sys/devices/system/cpu/cpu4/cpufreq/scaling_governor");
	system("sudo echo performance > /sys/devices/system/cpu/cpu4/cpufreq/scaling_governor");

	system("sudo chown linaro:linaro /sys/devices/system/cpu/cpu5/cpufreq/scaling_governor");
	system("sudo echo performance > /sys/devices/system/cpu/cpu5/cpufreq/scaling_governor");

	system("sudo cat /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq");
	system("sudo cat /sys/devices/system/cpu/cpu5/cpufreq/cpuinfo_cur_freq");
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



int test_burn_in_running(void)
{
	char cmd_buf[1024];
	char time_date[128];
	char time_time[128];
	if(app_lan_test(2) < 0)
	{
		printf("\033[1;31;40m lan connect Error!!!!! \033[0m\n");
		return -1;
	}

	auto_test_init();

	memset(time_date, 0, sizeof(time_date));
	memset(time_time, 0, sizeof(time_time));
	bndriver_get_data_time(time_date, time_time);

	memset(cmd_buf, 0, sizeof(cmd_buf));
	sprintf(cmd_buf, "echo \"[BSZH_Result][%s %s]burnin test:\" >> /home/linaro/burnintest-%s.txt", time_date, time_time, g_board_id);
	system(cmd_buf);

	while(1)
	{
		memset(time_date, 0, sizeof(time_date));
		memset(time_time, 0, sizeof(time_time));
		bndriver_get_data_time(time_date, time_time);
		memset(cmd_buf, 0, sizeof(cmd_buf));
		sprintf(cmd_buf, "echo \"[BSZH_One_Result][%s %s]:\" >> /home/linaro/burnintest-%s.txt", time_date, time_time, g_board_id);
		system(cmd_buf);

		memset(cmd_buf, 0, sizeof(cmd_buf));
		sprintf(cmd_buf, "sudo ping -c %d %s >> /home/linaro/burnintest-%s.txt", 10, g_config_service_ip, g_board_id);
		system(cmd_buf);

		memset(cmd_buf, 0, sizeof(cmd_buf));
		sprintf(cmd_buf, "echo \"CPU temp:\" >> /home/linaro/burnintest-%s.txt", g_board_id);
		system(cmd_buf);

		memset(cmd_buf, 0, sizeof(cmd_buf));
		sprintf(cmd_buf, "cat /sys/class/thermal/thermal_zone0/temp >> /home/linaro/burnintest-%s.txt", g_board_id);
		system(cmd_buf);

		bndriver_secondSleep(30);
	}
	return 0;
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


