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
#include <stdlib.h>

const char beep[] PROGMEM = "!L16 V15 cde";
const char weired_sound[]  PROGMEM = "!L16 V8 cdefgab>cbagfedc";

void play_sound(int8_t sound)
{
	if (sound == 0)
	{
		play_from_program_space(beep);
	}
	else if (sound == 1)
	{
		play_from_program_space(weired_sound);
	}
	else if (sound == 2)
	{
		play("!T240 L8 a gafaeada c+adaeafa >aa>bac#ada c#adaeaf4");
	}
}

void play_beep(void)
{
  play_from_program_space(beep);
}

int16_t limit(int16_t in, int16_t min, int16_t max)
{
    int16_t retVal;
	if (in < max && in > min)
	{
		retVal = in;
	}
	else if (in >= max)
	{
		retVal = max;
	}
	else if (in <= min)
	{
		retVal = min;
	}
	return retVal;
}

/* Stop robot. */
void stop(void)
{
  set_m1_speed(0);
  set_m2_speed(0);
  delay_ms(100);
}
