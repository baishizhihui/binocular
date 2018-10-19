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


int test_main_function(int cmd)
{
	return 0;
}
