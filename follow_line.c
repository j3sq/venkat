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
#include <stdint.h>

#include "markers.h"
#include "follow_line.h"
#include "utils.h"


/* Speed constants. */
#define MIN_COMMAND (-250)
#define MAX_COMMAND (250)

int currentSpeed = 50;
int targetSpeed = 50;
static int i = 0;
const int MAX_DELTA = 3;
const unsigned long MIN_RAMP_DELAY = 3;
unsigned long oldTimestamp = 0;


void clear_integral(void)
{
  i = 0;
}

void set_base_speed(int speed)
{
  targetSpeed = speed;
}

void follow_line(signed int l)
{
	int16_t p;
	int16_t d;
	static int16_t prev_p;
	int16_t motorDelta;
	int16_t left, right;
	//int16_t leftUnlim, rightUnlim;
  unsigned long timestamp = get_ms();

  if ((timestamp - oldTimestamp) >= MIN_RAMP_DELAY)
  {
    oldTimestamp = timestamp;
    if (currentSpeed != targetSpeed)
    {
      // Ramp up/down
      int delta = targetSpeed - currentSpeed;
      delta = limit(delta, -MAX_DELTA, MAX_DELTA);
      currentSpeed += delta;
    }
  }
   
	p = l-2000;
	i += p;
	d = p - prev_p;
	prev_p = p;

	// Use multiplications and right shifts to save execution time
	// The below line executes in 38 microseconds
	// read_line executes in about 3400 microseconds, hence the total saving is not very big..
	/*
	p >>= 3;
	motorDelta = ((p * 33) >> 7) + ((i >> 6) * 2 >> 6) + ((d * 13) >> 3);
	*/

	// High speed settings
	/*
	p >>= 3;
	motorDelta = ((p * 125) >> 7) + (i >> 10) + ((d * 225) >> 3);
	 */

	if (currentSpeed > 150)
	{
		// Below line executes in 121 microseconds
		motorDelta = p / 10 + ((d * 5) >> 1);
	}
	else if (currentSpeed > 100)
	{
		// Below line executes in 121 microseconds
		motorDelta = p / 10 + d;
	}
	else
	{
		// Below line executes in 121 microseconds
		motorDelta = p / 25 + d / 3;
	}

	//printf("P: %d, i: %d, d: %d\n",p,i,d);
	//leftUnlim  = SpeedLeft  + motorDelta;
	//rightUnlim = SpeedLeft  - motorDelta;
	left  = limit(currentSpeed  + motorDelta, MIN_COMMAND, MAX_COMMAND);
	right = limit(currentSpeed - motorDelta, MIN_COMMAND, MAX_COMMAND);

	set_m1_speed(left);
	set_m2_speed(right);
}

void follow_line_narrow(uint16_t * sensors)
{
  // Average over three middle sensors only
  signed int l =
    ((uint32_t)sensors[1] * 1000UL +
     (uint32_t)sensors[2] * 2000UL +
     (uint32_t)sensors[3] * 3000UL) /
    (sensors[1] + sensors[2] + sensors[3]);

  follow_line(l);
}

void follow_line_to_crossing_and_turn(FollowLineType dir)
{
	uint16_t sensors[5];
	uint16_t l;
	uint8_t crossingStartFound = FALSE;
	uint8_t crossingPassed = FALSE;

	while(!crossingPassed)
	{
		/* Update sensors. */
		(void)read_line(sensors,IR_EMITTERS_ON);

		/* Search for the crossing on the original sensor values. */
		if (sensors[LEFT_OUTHER_SENSOR]  > 300 &&
			sensors[RIGHT_OUTHER_SENSOR] > 300)
		{
			crossingStartFound = TRUE;
		}

		if (crossingStartFound &&
			sensors[LEFT_OUTHER_SENSOR] < 100 &&
			sensors[RIGHT_OUTHER_SENSOR] < 100)
		{
			crossingPassed = TRUE;
			play_beep();
		}
		//crossingFound = find_crossing(sensors);

		/* Create virtual sensor values to make the robot turn. */
		switch (dir)
		{
		case CLOSED_LOOP_RIGHT:
			/* Make robot "blind" on the left sensors. */
			sensors[LEFT_OUTHER_SENSOR] = 0;
			sensors[LEFT_INNER_SENSOR]  = 0;
			break;
		case CLOSED_LOOP_LEFT:
			/* Make robot "blind" on the right sensors. */
			sensors[RIGHT_OUTHER_SENSOR] = 0;
			sensors[RIGHT_INNER_SENSOR]  = 0;
			break;
		case CLOSED_LOOP_NARROW:
			sensors[LEFT_OUTHER_SENSOR]  = 0;
			sensors[RIGHT_OUTHER_SENSOR] = 0;
			break;
		}
		// Calculate the Average over three middle sensors only
		l = ((uint32_t)sensors[0] * 0UL +
			 (uint32_t)sensors[1] * 1000UL +
			 (uint32_t)sensors[2] * 2000UL +
			 (uint32_t)sensors[3] * 3000UL +
			 (uint32_t)sensors[4] * 4000UL) / (sensors[0] + sensors[1] + sensors[2] + sensors[3] + sensors[4]);

		follow_line(l);
	}
}

