#ifdef AVR_3PI
// The 3pi include file must be at the beginning of any program that
// uses the Pololu AVR library and 3pi.
#include <pololu/3pi.h>

// This include file allows data to be stored in program space.  The
// ATmega168 has 16k of program space compared to 1k of RAM, so large
// pieces of static data should be stored in program space.
#include <avr/pgmspace.h>

#else
#include "stubs.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "follow_line.h"
#include "utils.h"

void print_sensor_values( const uint16_t *sensordata)
{
  uint8_t i;
  stop();
  clear();
  
  for(i = 0; i < 5; i++)
  {
    lcd_goto_xy(0, i%2);
    print_long(i);
    print(":");
    print_long(sensordata[i]);
    if(i % 2)
    {
      wait_for_button(BUTTON_B);
      clear();
    }
  }

  wait_for_button(BUTTON_B);
}

void stop_with_message(char *message)
{
  stop();
  clear();
  lcd_goto_xy(0,0);
  print(message);
  while(1);
}

void print_two_messages(char *message1, char *message2)
{
  stop();
  clear();
  lcd_goto_xy(0,0);
  print(message1);
  lcd_goto_xy(0,1);
  print(message2);
}

void print_message_and_number(char *message, uint16_t number)
{
  stop();
  clear();
  lcd_goto_xy(0,0);
  print(message);
  lcd_goto_xy(0,1);
  print_long(number);
}

void print_message_and_two_numbers(char *message, uint16_t number1, uint16_t number2)
{
  stop();
  clear();
  lcd_goto_xy(0,0);
  print(message);
  lcd_goto_xy(0,1);
  print_long(number1);
  print(", ");
  print_long(number2);
}

    
void sensor_printing(void)
{
  uint16_t values[5];

  print_two_messages("B: 5sens","values");
  wait_for_button(BUTTON_B);
  // Print all 5 sensor values, press button B to scroll through
  while(1)
  {
    (void)read_line(values,IR_EMITTERS_ON);
    print_sensor_values(values);
  }
}
