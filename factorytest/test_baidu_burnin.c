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
		memset(time_date, 0, sizeof(time_date));
		memset(time_time, 0, sizeof(time_time));
		bndriver_get_data_time(time_date, time_time);
		memset(cmd_buf, 0, sizeof(cmd_buf));
		sprintf(cmd_buf, "echo \"[%s %s]:\" >> /home/linaro/burnintest-%s.txt", time_date, time_time, g_board_id);
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

int test_init(char *id)
{
	int size;
	printf("\033[1;31;40m factory test for baidu V1.0 - ID:%s\033[0m\n", id);
	memset(g_board_id, 0, sizeof(g_board_id));
	//strcpy(g_board_id, id);
	size = bndriver_file_getsize("/home/linaro/id.txt");
	if(size)
		bndriver_file_read("/home/linaro/id.txt", g_board_id, 0, size);

	app_gpio_init();

	memset(g_config_service_ip, 0, sizeof(g_config_service_ip));
	memset(g_config_wifi_name, 0, sizeof(g_config_wifi_name));
	memset(g_config_wifi_pwd, 0, sizeof(g_config_wifi_pwd));
	memset(g_config_lan_ip, 0, sizeof(g_config_lan_ip));
	memset(g_config_wifi_ip, 0, sizeof(g_config_wifi_ip));
	memset(g_config_wifi_gw, 0, sizeof(g_config_wifi_gw));

	size = bndriver_file_getsize("/home/linaro/ip.txt");
	if(size)
		bndriver_file_read("/home/linaro/ip.txt", g_config_service_ip, 0, size);	

	memset(g_board_test_outfile, 0, sizeof(g_board_test_outfile));
	sprintf(g_board_test_outfile, "%s-%s.txt", PATH_AUTOTEST_RESULT_FILE, g_board_id);
	printf("%s\n", g_board_test_outfile);

	memset(g_board_speed_outfile, 0, sizeof(g_board_speed_outfile));
	sprintf(g_board_speed_outfile, "%s-%s-speed.txt", PATH_AUTOTEST_RESULT_FILE, g_board_id);
	printf("%s\n", g_board_speed_outfile);

	printf("\033[1;32;40m config: \033[0m\n");	
	printf("\033[1;32;40m service ip=%s \033[0m\n",g_config_service_ip);
	//printf("\033[1;32;40m wifi name=%s \033[0m\n",g_config_wifi_name);
	//printf("\033[1;32;40m wifi password=%s \033[0m\n",g_config_wifi_pwd);
	//printf("\033[1;32;40m lan set ip=%s \033[0m\n",g_config_lan_ip);
	//printf("\033[1;32;40m wifi set ip=%s \033[0m\n",g_config_wifi_ip);
	//printf("\033[1;32;40m wifi gate way=%s \033[0m\n",g_config_wifi_gw);

	if(app_lan_update_time() <= 0)
	{
		printf("\033[1;31;40m update time fail \033[0m\n");
	}

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
	test_burn_in_running();
	return 1;	
}
