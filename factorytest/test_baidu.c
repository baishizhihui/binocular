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
extern char g_board_id[64];
extern char g_board_test_outfile[128];
extern char g_board_speed_outfile[128];
extern char g_config_service_ip[64];
extern char g_config_wifi_name[64];
extern char g_config_wifi_pwd[64];
extern char g_config_lan_ip[64];
extern char g_config_wifi_ip[64];
extern char g_config_wifi_gw[64];
extern char g_config_sn[64];

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
	sprintf(cmd_buf, "echo \"[%s %s]burnin test:\" >> /home/linaro/burnintest-%s.txt", time_date, time_time, g_board_id);
	system(cmd_buf);

	while(1)
	{
		memset(cmd_buf, 0, sizeof(cmd_buf));
		sprintf(cmd_buf, "ping -c %d %s >> /home/linaro/burnintest-%s.txt", 10, g_config_service_ip, g_board_id);
		system(cmd_buf);

		memset(cmd_buf, 0, sizeof(cmd_buf));
		sprintf(cmd_buf, "cat /sys/class/thermal/thermal_zone0/temp >> /home/linaro/burnintest-%s.txt", g_board_id);
		system(cmd_buf);

		bndriver_secondSleep(30);
	}
	return 0;
}

int test_init(char *id)
{
	int i;
	char buf[256];
	printf("\033[1;31;40m factory test for baidu V1.0 - ID:%s\033[0m\n", id);
	memset(g_board_id, 0, sizeof(g_board_id));
	strcpy(g_board_id, id);
	
	app_gpio_init();
	if(app_tfcard_mount(0) < 0)
	{
		printf("\033[1;31;40m TF card Error!!!!! \033[0m\n");
		return -1;
	}

	memset(g_config_service_ip, 0, sizeof(g_config_service_ip));
	memset(g_config_wifi_name, 0, sizeof(g_config_wifi_name));
	memset(g_config_wifi_pwd, 0, sizeof(g_config_wifi_pwd));
	memset(g_config_lan_ip, 0, sizeof(g_config_lan_ip));
	memset(g_config_wifi_ip, 0, sizeof(g_config_wifi_ip));
	memset(g_config_wifi_gw, 0, sizeof(g_config_wifi_gw));
	memset(g_config_sn, 0, sizeof(g_config_sn));

#if 1
	if(config_file_init(PATH_AUTOTEST_CONFIG_FILE) < 0)
	{
		printf("\033[1;31;40m config file Error!!!!! \033[0m\n");
		return -1;
	}
#else
	strcpy(g_config_sn, "SN20180921");
	strcpy(id, "9999");
#endif

	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%s%s", g_config_sn, id);
	printf("sn:%s\n",buf);
	bndriver_file_new("/home/linaro/id.txt");
	bndriver_file_add("/home/linaro/id.txt", buf, strlen(buf));

	memset(g_board_test_outfile, 0, sizeof(g_board_test_outfile));
	sprintf(g_board_test_outfile, "%s-%s.txt", PATH_AUTOTEST_RESULT_FILE, buf);
	printf("%s\n", g_board_test_outfile);

	memset(g_board_speed_outfile, 0, sizeof(g_board_speed_outfile));
	sprintf(g_board_speed_outfile, "%s-%s-speed.txt", PATH_AUTOTEST_RESULT_FILE, buf);
	printf("%s\n", g_board_speed_outfile);


	bndriver_file_new("/home/linaro/ip.txt");
	bndriver_file_add("/home/linaro/ip.txt", g_config_service_ip, strlen(g_config_service_ip));

	printf("\033[1;32;40m config: \033[0m\n");	
	printf("\033[1;32;40m service ip=%s \033[0m\n",g_config_service_ip);
	printf("\033[1;32;40m wifi name=%s \033[0m\n",g_config_wifi_name);
	printf("\033[1;32;40m wifi password=%s \033[0m\n",g_config_wifi_pwd);
	printf("\033[1;32;40m lan set ip=%s \033[0m\n",g_config_lan_ip);
	printf("\033[1;32;40m wifi set ip=%s \033[0m\n",g_config_wifi_ip);
	printf("\033[1;32;40m wifi gate way=%s \033[0m\n",g_config_wifi_gw);

#if 1
	if(app_lan_update_time() <= 0)
	{
		printf("\033[1;31;40m update time fail \033[0m\n");
	}
#endif
	return 1;
}

static int g_test_led_flag = 0;
void auto_test_timer_callback(void)
{
	if(g_test_led_flag)
	{
		g_test_led_flag = 0;
		app_red_led_set(0);
		app_green_led_set(0);
	}
	else
	{
		g_test_led_flag = 1;
		app_red_led_set(1);
		app_green_led_set(1);
	}
}


int test_main(int arg)
{
	int a,b;
	while(1)
	{
		printf("\033[1;32;40m *********************************** \033[0m\n");
		printf("\033[1;32;40m *  press number and enter start test \033[0m\n");		
		printf("\033[1;32;40m *  1. test all \033[0m\n");
		printf("\033[1;32;40m *  2. test camera \033[0m\n");	
		printf("\033[1;32;40m *  3. test usb \033[0m\n");
		printf("\033[1;32;40m *  4. test wifi \033[0m\n");
		printf("\033[1;32;40m *  5. test lan \033[0m\n");
		printf("\033[1;32;40m *  6. test tf card \033[0m\n");	
		printf("\033[1;32;40m *  7. test imu \033[0m\n");
		printf("\033[1;32;40m *  8. test burn-in test \033[0m\n");		
		printf("\033[1;32;40m *  9. test performance \033[0m\n");				
		printf("\033[1;32;40m *  0. test exit \033[1m\n");
		printf("\033[1;32;40m *********************************** \033[0m\n");	
		printf("\033[1;32;40m # \033[0m");
		scanf("%d",&a); 
		//printf("select %d\n", a);
		
		if(1 == a)
		{
			printf("\033[1;32;40m *********************************** \033[0m\n");
			printf("\033[1;31;40m *  start test all...... \033[0m\n");		
			printf("\033[1;32;40m *********************************** \033[0m\n");
			auto_test_init();
			app_fan_set(1);
			board_function_test(TEST_FUN_IMU, NULL);
			board_function_test(TEST_FUN_USB, NULL);
			board_function_test(TEST_FUN_WIFI, NULL);
			board_function_test(TEST_FUN_LAN, NULL);
			board_function_test(TEST_FUN_TF_CARD, NULL);
			board_function_result();
		}
		else if(2 == a)
		{
			printf("start camera:\n");	
			printf("--------------------------------\n");	
		}		
		else if(3 == a)
		{
			char ret_buf[128];
			printf("\033[1;32;40m *********************************** \033[0m\n");
			printf("\033[1;32;40m *  usb start test \033[0m\n");		
			printf("\033[1;32;40m *  1. test sda1 \033[0m\n");
			printf("\033[1;32;40m *  2. test sdb1 \033[0m\n");	
			printf("\033[1;32;40m *  3. test sdc1 \033[0m\n");			
			printf("\033[1;32;40m *  0. exit \033[1m\n");
			printf("\033[1;32;40m *********************************** \033[0m\n");	
			printf("\033[1;32;40m # \033[0m");
			while(1)
			{
				scanf("%d",&b);
				if(1 == b)
				{
					memset(ret_buf, 0, sizeof(ret_buf));
					if(app_usb_sda1_test(ret_buf) < 0)
					{
						printf("\033[1;31;40m [fail] test sda1 fail\033[0m \n");
					}
					else
					{
						printf("[success] test sda1 success\n");
					}					
				}
				else if(2 == b)
				{
					memset(ret_buf, 0, sizeof(ret_buf));
					if(app_usb_sdb1_test(ret_buf) < 0)
					{
						printf("\033[1;31;40m [fail] test sdb1 fail\033[0m \n");
					}
					else
					{
						printf("[success] test sdb1 success\n");
					}
				}
				else if(3 == b)
				{
					memset(ret_buf, 0, sizeof(ret_buf));
					if(app_usb_sdc1_test(ret_buf) < 0)
					{
						printf("\033[1;31;40m [fail] test sdc1 fail\033[0m \n");
					}
					else
					{
						printf("[success] test sdc1 success\n");
					}
				}				
				else if(0 == b)
				{
					break;
				}
			}
			
		
		}		
		else if(4 == a)
		{
			printf("start wifi test:\n");	
			printf("--------------------------------\n");			
			board_function_test(TEST_FUN_WIFI, NULL);
		}	
		else if(5 == a)
		{
			printf("start lan test:\n");	
			printf("--------------------------------\n");			
			board_function_test(TEST_FUN_LAN, NULL);
		}			
		else if(6 == a)
		{
			printf("start tf card test:\n");	
			printf("--------------------------------\n");			
			board_function_test(TEST_FUN_TF_CARD, NULL);
		}	
		else if(7 == a)
		{
			printf("start imu test:\n");	
			printf("--------------------------------\n");
			board_function_test(TEST_FUN_IMU, NULL);
		}	
		else if(8 == a)
		{
			printf("start burn-in test:\n");	
			printf("--------------------------------\n");			
			test_burn_in_running();
		}	
		else if(9 == a)
		{
			printf("start performance test:\n");	
			printf("--------------------------------\n");
			app_fan_set(1);
			board_function_test(TEST_FUN_WIFI_SPEED, NULL);
			board_function_test(TEST_FUN_LAN_SPEED, NULL);
			board_function_test(TEST_FUN_USB_SPEED, NULL);
			board_function_test(TEST_FUN_TFCARD_SPEED, NULL);
			board_function_test(TEST_FUN_EMMC_SPEED, NULL);
			app_all_speed_result();
		}	
		else if(10 == a)
		{
			printf("start gpio test:\n");	
			app_red_led_set(1);
			app_green_led_set(1);
		}	
		else if(88 == a)
		{
			printf("resv test:\n");	
			
		}	
					
		else if(0 == a)
		{
			break;
		}			
	}
	return 1;	
}
