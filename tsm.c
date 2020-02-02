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
#include "tsm.h"

const int8_t ROW_COUNT = 3;
const int8_t COL_COUNT = 4;
int8_t current_row_ = 0;
int8_t current_col_ = -1;
Direction current_direction_ = DIRECTION_WEST;
State state_ = STATE_FOLLOW_LINE_UNTIL_CROSSING;

// Goal parameters
Direction goal_direction_;
int8_t current_goal_ = 0;
int8_t goal_row_ = 0;
int8_t goal_col_ = 0;


void solve_challenge(void)
{
    current_goal_ = get_first_goal();
    set_goal_data(current_goal_);
    uint8_t done = FALSE;
    while(!done)
    {

        if (state_  == STATE_FOLLOW_LINE_UNTIL_CROSSING)
        {
            follow_line_until_crossing();   
        }
        else if (state_ == STATE_AT_CROSS)
        {
            // First: Get direction right
            // Second: A: For row goal, get to the correct row
            //         B: For col goal, get to the correct col
            // Third: Continue
            if (current_direction_ != goal_direction_ )
            {
                rotate_cw(); // ToDo: This can take 1, 2, or 3 turns to get into the right direction
                             // Optimize by rotating CW or CCW based on current direction and goal
            }
            else if(!check_row_col())
            {
                approach_goal_col_row();
            }
            else if (current_row_ == goal_row_ && current_col_ == goal_col_)
            {
                venkat(VENKAT_NO_ERR); // Done
            }
            else
            {
                state_ = STATE_FOLLOW_LINE_UNTIL_CROSSING;
            }
        }
    } 
}

void update_pos_after_crossing(void)
{
    if (current_direction_ == DIRECTION_WEST) 
    {
        current_col_++;
    }
    else if(current_direction_ == DIRECTION_NORTH)
    {
        current_row_++;
    }
    else if(current_direction_ == DIRECTION_EAST)
    {
        current_col_--;
    }
    else if(current_direction_ == DIRECTION_SOUTH)
    {
        current_row_--;
    }
    else
    {
        venkat(VENKAT_INVALID_CROSSING_DIRECTION);
    }

    play_beep();
    print_pos();

}

void follow_line_until_crossing(void)
{
    uint16_t sensors[5];
	static uint8_t crossing_entered = FALSE;
	static uint8_t crossing_passed = FALSE;
    int ll = read_line(sensors, IR_EMITTERS_ON);

    if (crossing_entered && crossing_passed)
    {
        // We have already crossed, clear both
        crossing_entered = FALSE;
        crossing_passed = FALSE;
        follow_line(ll);
        return;
    }

    if (sensors[LEFT_OUTHER_SENSOR]  > 300 &&
    sensors[RIGHT_OUTHER_SENSOR] > 300)
    {
        crossing_entered = TRUE;
    }

    if (crossing_entered &&
        sensors[LEFT_OUTHER_SENSOR] < 100 &&
        sensors[RIGHT_OUTHER_SENSOR] < 100)
    {
        crossing_passed = TRUE;
        update_pos_after_crossing();
        state_ = STATE_AT_CROSS;
    }

    follow_line(ll);
}

uint8_t check_end(uint8_t row, uint8_t col)
{
    return col == COL_COUNT;
}

uint8_t get_first_goal()
{
    // return COL_COUNT + 1;
    return 6;
}

void set_goal_data(uint8_t center)
{
    if (center == 0) center = 2 * (ROW_COUNT + COL_COUNT);
    
    if (center <= COL_COUNT)
    {
        goal_direction_ = DIRECTION_SOUTH;
        goal_col_ = center - 1;
        goal_row_ = 0;
    }
    else if (center <= COL_COUNT + ROW_COUNT)
    {
        goal_direction_ = DIRECTION_WEST;
        goal_col_ = COL_COUNT - 1;
        goal_row_ = center - COL_COUNT - 1;
    }
    else if (center <= COL_COUNT + ROW_COUNT + COL_COUNT)
    {
        goal_direction_ = DIRECTION_NORTH;
        goal_col_ = 2 * COL_COUNT + ROW_COUNT - center;
        goal_row_ = ROW_COUNT - 1;
    }
    else if (center <= 2 * (ROW_COUNT + COL_COUNT))
    {
        goal_direction_ = DIRECTION_EAST;
        goal_col_ = 0;
        goal_row_ = 2 * (ROW_COUNT + COL_COUNT) - center;
    }
    else
    {
        venkat(VENKAT_INVALID_CENTER);
    }
    
}


void rotate_cw()
{
    uint16_t sensors[5];
	uint16_t l;
    uint8_t done = 0;

    clear();
    lcd_goto_xy(0,0);
    print("Vnkt");
    print("cw");
    while(!done)
    {
        (void) read_line(sensors, IR_EMITTERS_ON);
        l = ((uint32_t)sensors[2] * 2000UL +
			 (uint32_t)sensors[3] * 3000UL +
			 (uint32_t)sensors[4] * 4000UL) / (sensors[2] + sensors[3] + sensors[4]);
        follow_line(l);
        delay_ms(100);
        if (sensors[LEFT_OUTHER_SENSOR] < 100 && sensors[RIGHT_OUTHER_SENSOR] < 100)
        {
            done = TRUE;
            play_sound(1);
            if (current_direction_ == DIRECTION_WEST)
            {
                current_direction_ = DIRECTION_NORTH;
            }
            else
            {
                current_direction_++;
            }
            
        }
    }
    return;
}

void rotate_ccw()
{
    uint16_t sensors[5];
	uint16_t l;
    uint8_t done = 0;

    clear();
    lcd_goto_xy(0,0);
    print("Vnkt");
    print("ccw");

    while(!done)
    {
        (void) read_line(sensors, IR_EMITTERS_ON);
        l = ((uint32_t)sensors[0] * 2000UL +
			 (uint32_t)sensors[1] * 3000UL +
			 (uint32_t)sensors[2] * 4000UL) / (sensors[0] + sensors[1] + sensors[2]);
        follow_line(l);
        if (sensors[LEFT_OUTHER_SENSOR] < 100 && sensors[RIGHT_OUTHER_SENSOR] < 100)
        {
            done = TRUE;
            if (current_direction_ == DIRECTION_WEST)
            {
                current_direction_ = DIRECTION_NORTH;
            }
            else
            {
                current_direction_++;
            }
        }
    }
}

void venkat(Venkat_type err)
{
    play_beep();
    lcd_goto_xy(0,0);
    clear();
    print("Vnkt!");
    lcd_goto_xy(0,1);
    print_long(err);
    play_beep();
    stop();
    while (TRUE);
    
    return;

}

void print_pos(void)
{
    lcd_goto_xy(0,0);
    clear();
    print_long(current_row_);
    print(":");
    print_long(current_col_);
}

uint8_t check_row_col(void)
{
    if ((goal_direction_ == DIRECTION_EAST) || (goal_direction_== DIRECTION_WEST))
    {
        // Center is a row center
        return current_row_ == goal_row_;
    } 
    else if((goal_direction_ == DIRECTION_NORTH) || (goal_direction_== DIRECTION_SOUTH))
    {
        // Center is a col center
        return current_col_ == goal_col_;
    }
    else
    {
        /* code */
        venkat(VENKAT_INVALID_GOAL_DIRECTION);
        return FALSE;
    }
    
}

void approach_goal_col_row(void)
{
    if (goal_direction_ == DIRECTION_WEST || goal_direction_ == DIRECTION_EAST)
    {
        // Row goal
        uint8_t delta = (goal_row_ - current_row_);
        if (delta > 0)
        {
            rotate_cw();// Go up
            state_ = STATE_FOLLOW_LINE_UNTIL_CROSSING;
        }
        else if (delta < 0)
        {
            rotate_ccw();// Go down
            state_ = STATE_FOLLOW_LINE_UNTIL_CROSSING;
        }
        else
        {
            // delta = 0, shouldn't be here
            venkat(VENKAT_INVALID_DELTA);
        }
    }
    else if(goal_direction_ == DIRECTION_NORTH || goal_direction_ == DIRECTION_SOUTH)
    {
        // Col goal
        uint8_t delta = (goal_col_ - current_col_);
        if (delta > 0)
        {
            rotate_ccw();// Go right
            state_ = STATE_FOLLOW_LINE_UNTIL_CROSSING;
        }
        else if (delta < 0)
        {
            rotate_cw();// Go left
            state_ = STATE_FOLLOW_LINE_UNTIL_CROSSING;
        }
        else
        {
            // delta = 0, shouldn't be here
            venkat(VENKAT_INVALID_DELTA);
        }
    }
    else
    {
        venkat(VENKAT_INVALID_GOAL_DIRECTION + 11);
    }
    return;
}

void sanity_check(void)
{
    if (current_col_ < -1 || current_col_ > COL_COUNT || current_row_ < -1 || current_row_ > ROW_COUNT)
    {
        venkat(VENKAT_OUTSIDE_ARENA);
    }
}