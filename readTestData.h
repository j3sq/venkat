/*
 * readTestData.h
 *
 *  Created on: Nov 25, 2017
 *      Author: fredrik
 */

#ifndef READTESTDATA_H_
#define READTESTDATA_H_

#include <stdint.h>

typedef struct
{
	uint16_t v[5];
}stackElementType;

void reset_stack (void);
void reset_fifo(void);
void push_on_stack(stackElementType e);
uint8_t pop_from_stack(stackElementType *p);
void push_on_fifo(stackElementType e);
void read_line_from_text(const char *file, void (*f)(const uint16_t v[5]));
uint8_t pop_from_fifo(stackElementType *p);

#endif /* READTESTDATA_H_ */

