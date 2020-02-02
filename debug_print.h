
#ifndef DEBUG_PRINT_H_
#define DEBUG_PRINT_H_

#include <stdint.h>

void stop_with_message(char *message);
void print_two_messages(char *message1, char *message2);
void print_message_and_number(char *message, uint16_t number);
void print_message_and_two_numbers(char *message, uint16_t number1, uint16_t number2);
void sensor_printing(void);

#endif /* DEBUG_PRINT_H_ */
