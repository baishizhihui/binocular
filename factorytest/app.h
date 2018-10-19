#include "test.h"

//#define ADD_IMU_TEST

extern int imu_test_main(void);

extern int config_file_init(char *file);
extern int cpu_freq_init(void);

extern int app_tfcard_test_main(int flag);
extern int app_tfcard_mount(int flag);
extern int app_tfcard_speed_test(int flag);

extern void app_gpio_init(void);
extern void app_fan_set(int flag);
extern void app_red_led_set(int flag);
extern void app_green_led_set(int flag);

extern int app_usb_sda1_test(char *buf);
extern int app_usb_sdb1_test(char *buf);
extern int app_usb_sdc1_test(char *buf);
extern int app_usb_sda1_speed_test(int flag);
extern int app_usb_sdb1_speed_test(int flag);
extern int app_usb_sdc1_speed_test(int flag);

extern int app_lan_test(int pingcount);
extern int app_wifi_test(int pingcount);
extern int app_lan_speed_test(void);
extern int app_wifi_speed_test(void);
extern int app_lan_update_time(void);

