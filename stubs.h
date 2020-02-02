#ifndef STUBS_H_
#define STUBS_H_

#include <stdint.h>

/* Stub interface file for test purpose. */
enum{IR_EMITTERS_ON};
enum{MODE_10_BIT};
enum{TEMP_SENSOR};
enum{ANY_BUTTON, BUTTON_A, BUTTON_B};

#define PROGMEM

extern unsigned int read_line(uint16_t *sensorValues, unsigned char readMode);
extern void set_motors(int m1Speed, int m2Speed);
extern void set_m1_speed(int speed);
extern void set_m2_speed(int speed);
extern void pololu_3pi_init(int timeout);
extern void set_analog_mode(unsigned char mode);
extern void calibrate_line_sensors(unsigned char readMode);
extern void delay_ms(int delay);
extern void delay_us(int delay);
extern void delay(int delay);
extern unsigned long get_ms(void);
extern int analog_read_millivolts(unsigned char ch);
extern void clear(void);
extern void lcd_goto_xy(unsigned char x, unsigned char y);
extern void print(char *s);
extern void print_long(long value);
extern void lcd_init_printf_with_dimensions(unsigned char x, unsigned char y);
extern int button_is_pressed(unsigned char button);
extern void serial_set_baud_rate(int b);
extern void serial_send_blocking(char *s, int l);
extern void play_from_program_space(const char *s);
extern unsigned char wait_for_button(unsigned char buttons);
extern int16_t read_battery_millivolts_3pi(void);


extern void getRawData (uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c);

#endif /* STUBS_H_ */
