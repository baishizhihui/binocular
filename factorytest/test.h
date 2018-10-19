#define PATH_AUTOTEST_CONFIG_FILE "/tmp/sdcard/autotestconfig.txt"
#define PATH_AUTOTEST_RESULT_FILE "/tmp/sdcard/boteyeone-"

#define TEST_FUN_WIFI		1
#define TEST_FUN_LAN		(TEST_FUN_WIFI+1)
#define TEST_FUN_TF_CARD	(TEST_FUN_LAN+1)
#define TEST_FUN_USB		(TEST_FUN_TF_CARD+1)
#define TEST_FUN_CAMERA		(TEST_FUN_USB+1)

#define TEST_FUN_AUDIO		(TEST_FUN_CAMERA+1)
#define TEST_FUN_TYPEC		(TEST_FUN_AUDIO+1)
#define TEST_FUN_HDMI		(TEST_FUN_TYPEC+1)
#define TEST_FUN_UART		(TEST_FUN_HDMI+1)
#define TEST_FUN_MOTOR		(TEST_FUN_UART+1)

#define TEST_FUN_FAN		(TEST_FUN_MOTOR+1)
#define TEST_FUN_ONOFF_KEY	(TEST_FUN_FAN+1)
#define TEST_FUN_IMU		(TEST_FUN_ONOFF_KEY+1)

#define TEST_FUN_WIFI_SPEED	(TEST_FUN_IMU+1)
#define TEST_FUN_LAN_SPEED	(TEST_FUN_WIFI_SPEED+1)
#define TEST_FUN_USB_SPEED	(TEST_FUN_LAN_SPEED+1)
#define TEST_FUN_TFCARD_SPEED	(TEST_FUN_USB_SPEED+1)
#define TEST_FUN_EMMC_SPEED	(TEST_FUN_TFCARD_SPEED+1)

#define TEST_FUN_MAX		(TEST_FUN_EMMC_SPEED+1)



extern int test_init(char *id);
extern int test_main(int arg);
extern int test_main_function(int cmd);
extern int test_burn_in_running(void);
extern int app_dd_speed_test(char *path, char *outputfile);

extern int board_function_test(int flag, char *buf);
extern int net_file_rw_test(void);
extern int net_file_new(void);

extern void auto_test_init(void);
extern int fun_test(int flag, char *buf);
extern void auto_test_timer_callback(void);
extern int file_rw_test(char *path);
extern int board_function_result(void);
extern int app_all_speed_result(void);

