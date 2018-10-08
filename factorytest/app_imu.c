#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include <time.h>


#define ATTRIBUTES_PATH	"/sys/devices/platform/ff3d0000.i2c/i2c-4/4-0068/iio:device0/"

int inv_imu_enable(void)
{
	system("sudo chown -R linaro:linaro /sys/devices/platform/ff3d0000.i2c/i2c-4/4-0068/iio:device0");
	system("sudo chown linaro:linaro /dev/iio:device0");

	system("echo 1 > /sys/devices/platform/ff3d0000.i2c/i2c-4/4-0068/iio:device0/scan_elements/in_anglvel_x_en");
	system("echo 1 > /sys/devices/platform/ff3d0000.i2c/i2c-4/4-0068/iio:device0/scan_elements/in_anglvel_y_en");
	system("echo 1 > /sys/devices/platform/ff3d0000.i2c/i2c-4/4-0068/iio:device0/scan_elements/in_anglvel_z_en");
	system("echo 1 > /sys/devices/platform/ff3d0000.i2c/i2c-4/4-0068/iio:device0/scan_elements/in_accel_x_en");
	system("echo 1 > /sys/devices/platform/ff3d0000.i2c/i2c-4/4-0068/iio:device0/scan_elements/in_accel_y_en");
	system("echo 1 > /sys/devices/platform/ff3d0000.i2c/i2c-4/4-0068/iio:device0/scan_elements/in_accel_z_en");

	system("echo 1 > /sys/devices/platform/ff3d0000.i2c/i2c-4/4-0068/iio:device0/accl_enable");
	system("echo 1 > /sys/devices/platform/ff3d0000.i2c/i2c-4/4-0068/iio:device0/gyro_enable");

	system("echo 1 > /sys/devices/platform/ff3d0000.i2c/i2c-4/4-0068/iio:device0/buffer/enable");
}

int inv_imu_disable(void)
{
	system("echo 0 > /sys/devices/platform/ff3d0000.i2c/i2c-4/4-0068/iio:device0/scan_elements/in_anglvel_x_en");
	system("echo 0 > /sys/devices/platform/ff3d0000.i2c/i2c-4/4-0068/iio:device0/scan_elements/in_anglvel_y_en");
	system("echo 0 > /sys/devices/platform/ff3d0000.i2c/i2c-4/4-0068/iio:device0/scan_elements/in_anglvel_z_en");
	system("echo 0 > /sys/devices/platform/ff3d0000.i2c/i2c-4/4-0068/iio:device0/scan_elements/in_accel_x_en");
	system("echo 0 > /sys/devices/platform/ff3d0000.i2c/i2c-4/4-0068/iio:device0/scan_elements/in_accel_y_en");
	system("echo 0 > /sys/devices/platform/ff3d0000.i2c/i2c-4/4-0068/iio:device0/scan_elements/in_accel_z_en");

	system("echo 0 > /sys/devices/platform/ff3d0000.i2c/i2c-4/4-0068/iio:device0/accl_enable");
	system("echo 0 > /sys/devices/platform/ff3d0000.i2c/i2c-4/4-0068/iio:device0/gyro_enable");

	system("echo 0 > /sys/devices/platform/ff3d0000.i2c/i2c-4/4-0068/iio:device0/buffer/enable");
}	


struct inv_imu_data {
	unsigned short accl_x;
	unsigned short accl_y;
	unsigned short accl_z;
	unsigned short temp;
	unsigned short gyro_x;
	unsigned short gyro_y;
	unsigned short gyro_z;
	unsigned short dummy;
	unsigned long long temp1;
	unsigned long long temp2;
	//struct timeval tv;
};

static int hex_dump(struct inv_imu_data *pimudate, int len)
{
	int i;
	int min = 0;
	int max = 0;
	printf("acc=(%d,%d,%d),gypo=(%d,%d,%d),time=%lld,%lld\n",pimudate->accl_x, pimudate->accl_y, pimudate->accl_z,pimudate->gyro_x, pimudate->gyro_y, pimudate->gyro_z,pimudate->temp1,pimudate->temp2);
	//printf("gypo x,y,z=(%d,%d,%d)\n",pimudate->gyro_x, pimudate->gyro_y, pimudate->gyro_z);
	//printf("time=%lld,%lld\n",pimudate->temp1,pimudate->temp2);
	//hex_dump2((unsigned char*)pimudate, len);
	//if((pimudate->accl_x == 0)||(pimudate->accl_y == 0)||(pimudate->accl_z == 0))
	if(pimudate->gyro_x > 65000)
	{
		max++;
	}
	else if(pimudate->gyro_x < 100)
	{
		min++;
	}

	if(pimudate->gyro_y > 65000)
	{
		max++;
	}
	else if(pimudate->gyro_y < 100)
	{
		min++;
	}

	if(pimudate->gyro_z > 65000)
	{
		max++;
	}
	else if(pimudate->gyro_z < 100)
	{
		min++;
	}
	if(( max > 2 )||(min > 2))
	{
		return -1;
	}

	return 1;
}

int imu_test_main(void)
{
	int fd, rd, ret;
	int count;
	int successcount = 0;
	struct timespec ts;
	struct inv_imu_data data;
	FILE *fp;
	
	fp = fopen("/sys/devices/platform/ff3d0000.i2c/i2c-4/4-0068/iio:device0/scan_elements/in_anglvel_x_en", "r");
	if (!fp) {
		printf("open fail\n");
		return -1;
	}
	fclose(fp);

	inv_imu_enable();
	fd = open("/dev/iio:device0", O_RDONLY);

	count = 10;
	while (1) {
		struct pollfd pfd = {
			.fd = fd,
			.events = POLLIN,
		};		
		ret = poll(&pfd, 1, 10);
		if (ret < 0) {
			printf("poll fail...\n");
			break;
		}

		rd = read(fd, &data, sizeof(struct inv_imu_data));
		if(hex_dump(&data, sizeof(struct inv_imu_data)) > 0)
		{
			successcount++;
		}
		sleep(1);
		count--;
		if(count <= 0)
		{
			break;
		}
	}

	close(fd);
	inv_imu_disable();
	if(successcount > 5)
	{
		printf("imu test success\n");
		return 0;
	}
	return -1;
}
