/*
 * markers.c
 *
 *  Created on: Nov 24, 2017
 *      Author: fredrik
 */

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

#include "markers.h"
#include "utils.h"
#include <stdio.h>

uint8_t find_crossing(uint16_t const *sensordata){
	uint8_t retVal = 0;

	return retVal;
}

