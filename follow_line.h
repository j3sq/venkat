
#ifndef FOLLOW_LINE_H_
#define FOLLOW_LINE_H_

#include <stdint.h>

/* Used to turn closed loop in a crośsing. */
typedef enum
{
	CLOSED_LOOP_RIGHT,
	CLOSED_LOOP_LEFT,
	CLOSED_LOOP_NARROW,
}FollowLineType;


void set_base_speed(int speed);
void clear_integral(void);
void follow_line(signed int l);
void follow_line_narrow(uint16_t * sensors);
void follow_line_to_crossing_and_turn(FollowLineType dir);

#endif /* FOLLOW_LINE_H_ */
