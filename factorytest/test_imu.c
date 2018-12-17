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
	board_function_test(TEST_FUN_IMU, NULL);
	return 1;	
}


int test_main_function(int cmd)
{
	return 0;
}
