
#ifndef UTILS_H_
#define UTILS_H_

#include <stdint.h>

enum
{
	LEFT_OUTHER_SENSOR,
	LEFT_INNER_SENSOR,
	MID_SENSOR,
	RIGHT_INNER_SENSOR,
	RIGHT_OUTHER_SENSOR,
};

#define TRUE (0 == 0)
#define FALSE (0 != 0)

void play_beep(void);
void stop(void);
int16_t limit(int16_t in, int16_t min, int16_t max);
void play_sound(int8_t sound);

#endif /* UTILS_H_ */
