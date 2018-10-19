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

int main(int argc, char **argv)
{
	int ret;
	char buf[256];
	memset(buf, 0, sizeof(buf));
	if(argc == 2)
	{
		sprintf(buf, "%s", argv[1]);
		printf("%s\n", buf);
	}
	else if(argc == 3)
	{
		test_main_function(atoi(argv[2]));
		return 1;
	}
	ret = test_init(buf);
	if(ret > 0)
	{
		test_main(argc);
	}
	return 1;
}
