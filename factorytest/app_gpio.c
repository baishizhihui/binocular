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


#define RK_GPIO_NR(bank, nr) ((bank)*32 + nr)

#define FAN_EN_PIN	RK_GPIO_NR(2, 16+2)	//gpio2c2
#define LED_GREEN_PIN	RK_GPIO_NR(2, 0+4)	//gpio2a4
#define LED_RED_PIN	RK_GPIO_NR(2, 0+5)	//gpio2a5

void app_gpio_init(void)
{
	system("sudo chown linaro:linaro /sys/class/gpio/export");
	bndriver_secondSleep(1);
	bndriver_gpio_pin_init(FAN_EN_PIN, 1);//fan
	bndriver_gpio_pin_init(LED_GREEN_PIN, 1);//led1
	bndriver_gpio_pin_init(LED_RED_PIN, 1);//led2

	system("sudo chown linaro:linaro /sys/class/gpio/export");
	bndriver_secondSleep(1);
	bndriver_gpio_pin_init(FAN_EN_PIN, 1);//fan
	bndriver_gpio_pin_init(LED_GREEN_PIN, 1);//led1
	bndriver_gpio_pin_init(LED_RED_PIN, 1);//led2

	bndriver_gpio_write(FAN_EN_PIN, 1);
	bndriver_gpio_write(LED_RED_PIN, 1);
	bndriver_gpio_write(LED_GREEN_PIN, 1);
}

void app_fan_set(int flag)
{
	if(flag)
	{
		bndriver_gpio_write(FAN_EN_PIN, 1);
	}
	else
	{
		bndriver_gpio_write(FAN_EN_PIN, 0);
	}
}

void app_red_led_set(int flag)
{
	if(flag)
	{
		bndriver_gpio_write(LED_RED_PIN, 1);
	}
	else
	{
		bndriver_gpio_write(LED_RED_PIN, 0);
	}
}

void app_green_led_set(int flag)
{
	if(flag)
	{
		bndriver_gpio_write(LED_GREEN_PIN, 1);
	}
	else
	{
		bndriver_gpio_write(LED_GREEN_PIN, 0);
	}
}




