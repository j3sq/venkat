/*
 * readTestData.c
 *
 *  Created on: Nov 25, 2017
 *      Author: fredrik
 */

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "readTestData.h"
#include "utils.h"

#define STACK_SIZE (1000)
stackElementType storage[STACK_SIZE];
stackElementType fifo[STACK_SIZE];
int16_t stackIndex;
int16_t writeIndex;
int16_t readIndex;

void reset_stack (void)
{
	stackIndex = 0;
}
void reset_fifo (void)
{
	writeIndex = 0;
	readIndex = 0;
}

void push_on_fifo(stackElementType e)
{
	if (stackIndex < STACK_SIZE)
	{
		fifo[writeIndex] = e;
		writeIndex++;
	}
	else
	{
		print_long(row);
		print(":");
		print_loong(col);
	}
}
uint8_t pop_from_fifo(stackElementType *p)
{
	stackElementType e;
	uint8_t retVal;
	if (readIndex < writeIndex)
	{
		e = fifo[readIndex];
		p->v[0] = e.v[0];
		p->v[1] = e.v[1];
		p->v[2] = e.v[2];
		p->v[3] = e.v[3];
		p->v[4] = e.v[4];
		readIndex++;
		retVal = TRUE;
	}
	else
	{
		printf("No data left in fifo!\n");
		retVal = FALSE;
	}
	return retVal;
}


void push_on_stack(stackElementType e)
{
	if (stackIndex < STACK_SIZE)
	{
		storage[stackIndex] = e;
		stackIndex++;
	}
	else
	{
		printf("Data stack is full!\n");
	}
}

uint8_t pop_from_stack(stackElementType *p)
{
	stackElementType e;
	uint8_t retVal;
	stackIndex--;
	if (stackIndex > 0)
	{
		stackIndex--;
		e = storage[stackIndex];
		p->v[0] = e.v[0];
		p->v[1] = e.v[1];
		p->v[2] = e.v[2];
		p->v[3] = e.v[3];
		p->v[4] = e.v[4];
		retVal = TRUE;
	}
	else
	{
		printf("No data left in stack!\n");
		retVal = FALSE;
	}
	return retVal;
}

void read_line_from_text(const char *file, void (*f)(const uint16_t v[5]))
{
	FILE *myFile = NULL;
	char line[1000];
	int value[5];
	uint16_t sensor[5];
	myFile = fopen(file, "r");

	if (NULL == myFile)
	{
		printf("Error when reading %s\n", file);
		return;
	}

	while (NULL != fgets(line, sizeof(line), myFile))
	{
		if (0 == strcmp("  --- START ---\n", line))
		{
			/* Ignore start tag*/
			//printf("StartTag found!\n");
		}
		else if (5 == sscanf(line, "%d,%d,%d,%d,%d\n",&value[0],&value[1],&value[2],&value[3],&value[4]))
		{
			//printf("Values %d, %d, %d, %d, %d\n", value[0], value[1], value[2], value[3], value[4]);
			sensor[0] = (uint16_t)value[0];
			sensor[1] = (uint16_t)value[1];
			sensor[2] = (uint16_t)value[2];
			sensor[3] = (uint16_t)value[3];
			sensor[4] = (uint16_t)value[4];
			
			/* Here we call the function callout which call the function to test. */
			(*f)(sensor);
		}
		else if (0 == strcmp("  --- END ---", line))
		{
			/* Ignore stop tag*/
			//printf("StopTag found!\n");
		}
	}
	if (0 != fclose(myFile))
	{
		printf("Failure on closing %s\n", file);
	}
}

