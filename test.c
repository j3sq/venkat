/*
 * test.c
 *
 * Testing plate to try out things before loading into target...
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stubs.h"
#include "utils.h"
#include "markers.h"
#include "nodesLog.h"
#include "debug_print.h"
#include "readTestData.h"

/* Minimalist test framework. */
uint32_t nbrOfFaults = 0;
uint32_t nbrOfTests = 0;

void test_assert_func (uint8_t cond, int line, char *filePtr)
{
	if (!cond)
	{
		/* Test case failed! */
		nbrOfFaults++;
		nbrOfTests++;
		printf("Test case failed! Line: %d\n", line);
	}
	else
	{
		nbrOfTests++;
	}
}

void test_summary (void)
{
	printf("%d tests executed, %d failed. See above. \n", nbrOfTests, nbrOfFaults);
}

#define test_assert(cond)  (test_assert_func (cond, __LINE__, __FILE__))

/* Stub section. */
int16_t read_battery_millivolts_3pi(void)
{
	return 0;
}
int set_motors_m1Speed;
int set_motors_m2Speed;
void set_motors(int m1Speed, int m2Speed){
	set_motors_m1Speed = limit(m1Speed, -250, 250);
	set_motors_m2Speed = limit(m2Speed, -250, 250);
}
void set_m1_speed(int speed)
{
	set_motors_m1Speed = limit(speed, 0 ,250);
	//printf("Left speed: %d\n", speed);
}
void set_m2_speed(int speed)
{
	set_motors_m1Speed = limit(speed, 0, 250);
	//printf("Right speed: %d\n", speed);
}


void sim_callout(const uint16_t v[5], uint8_t expected)
{
	stackElementType e;
	int i = 0;
	for (i = 0; i < 5; i++)
	{
		e.v[i] = v[i];
	}
	push_on_fifo(e);
}

void sim_callout_reverse(const uint16_t v[5], uint8_t expected)
{
	stackElementType e;
	int i = 0;
	for (i = 0; i < 5; i++)
	{
		e.v[i] = v[i];
	}
	push_on_stack(e);
}


void clear(void)
{

}

void lcd_goto_xy(unsigned char x, unsigned char y)
{

}

void print(char *s)
{
	printf("%s\n",s);
}
void print_long(long value)
{
	printf("print_long: %ld\n", value);
}

unsigned char wait_for_button(unsigned char buttons)
{
	return 0;
}



void pololu_3pi_init(int timeout){}
void set_analog_mode(unsigned char mode){}
void calibrate_line_sensors(unsigned char readMode){}
void delay_ms(int delay){}
void lcd_init_printf_with_dimensions(unsigned char x, unsigned char y){}
int button_is_pressed(unsigned char button){return 0;}
void serial_set_baud_rate(int b){}
void serial_send_blocking(char *s, int l){}
void play_from_program_space(const char *s)
{
    if (0 == strcmp(s, "!L16 V15 fff"))
	{
		printf("Crossing seen!\n");
	}
	else
	{
		printf("%s\n", s);
	}
}
void delay_us(int delay){}
void delay(int delay)
{
	printf("Delay: %d\n", delay);
}

unsigned long get_ms(void)
{
  static unsigned int time = 0;
  time += 100;
  return time;
}


void reverse_callout(const uint16_t v[5], uint8_t expected)
{
	stackElementType e;
	e.v[0] = v[0];
	e.v[1] = v[1];
	e.v[2] = v[2];
	e.v[3] = v[3];
	e.v[4] = v[4];
	push_on_stack(e);
}

void replay_stack_content (void (*f)(const uint16_t v[5], uint8_t expected), uint8_t expected)
{
	stackElementType e;
	uint16_t values[5];
	while (pop_from_stack(&e))
	{
		values[0] = e.v[0];
		values[1] = e.v[1];
		values[2] = e.v[2];
		values[3] = e.v[3];
		values[4] = e.v[4];
		(*f)(values, expected);
	}
}

unsigned int read_line(uint16_t *sensorValues, unsigned char readMode){
	uint16_t retVal;

    retVal =  ((uint32_t)sensorValues[0] * 0UL +
    		   (uint32_t)sensorValues[1] * 1000UL +
			   (uint32_t)sensorValues[2] * 2000UL +
			   (uint32_t)sensorValues[3] * 3000UL +
			   (uint32_t)sensorValues[4] * 4000UL) / (sensorValues[0] + sensorValues[1] + sensorValues[2] + sensorValues[3] +sensorValues[4]);

	return retVal;
}


uint8_t crossingDetected;
void crossing_callout(const uint16_t v[5])
{
	if (find_crossing(v))
	{
		printf("Crossing found!\n");
		crossingDetected = TRUE;
	}
}

/* Example test case for testing the function to detect a crossing in 
 * the grid. Note that the timing between two lines is different in 
 * reality. */
void crossing_test(void)
{
	crossingDetected = FALSE;
	read_line_from_text("RecordedData/Crossing.txt",crossing_callout);
	test_assert(TRUE == crossingDetected);
}

/* Start of test program. */
int main(int argc, char **argv)
{
	/* This is a way to test usable functions.. */
	set_motors(10,-10);
	test_assert(10  == set_motors_m1Speed);
	test_assert(-10 == set_motors_m2Speed);
	
    /* Run a complete set of tests */
    crossing_test();

	return 0;
}

