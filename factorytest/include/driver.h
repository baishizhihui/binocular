//common
extern int bndriver_msSleep(long ms);
extern int bndriver_secondSleep(long second);
extern void bndriver_debug_print_string(char *pid, int count ,char *pbuf);
extern int bndriver_string_to_array(char *pin, int len, unsigned char *pout);
extern int bndriver_get_data_string(unsigned char *poutdata, unsigned char *pouttime);
extern int bndriver_string_to_hex(char *pin);
extern int bndriver_string_to_oct(char *pin);
extern int bndriver_super_system(const char * cmd, char *retmsg, int msg_len);
extern int bndriver_get_data_time(unsigned char *poutdata, unsigned char *pouttime);

//crc
extern int bndriver_crc_get_crc16(unsigned char *buf, int len);

//gpio
extern int bndriver_gpio_pin_init(int pin, int dir);/*dir 1:output,0:input*/
extern int bndriver_gpio_write(int pin, int value);/*value 1:high,0:low*/

//uart
extern int bndriver_uart_data_init(char *uart, int speed);
extern int bndriver_uart_data_write(int fd, char *buf, int len);
extern int bndriver_uart_data_read(int fd, char *pbuf, int *plen);

//file
extern int bndriver_file_new(char *file);
extern int bndriver_file_add(char *file, char *buf, int len);
extern int bndriver_file_getsize(char *file);
extern int bndriver_file_read(char *file, char *buf, int offset, int len);
extern int bndriver_file_context_num(char *file);

//net
extern int bndriver_check_wlan(void);
extern int bndriver_check_lan(void);
