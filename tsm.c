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
#include "nodesLog.h"
#include "tsm.h"

const int8_t ROW_COUNT = 3;
const int8_t COL_COUNT = 4;
int8_t current_row_ = 0;
int8_t current_col_ = -1;
Direction current_direction_ = DIRECTION_WEST;
State state_ = STATE_FOLLOW_LINE_UNTIL_CROSSING;
Gate_state gate_state_ = GATE_STATE_NA;
uint8_t next_center_ = 0;
uint8_t out_on_delivery_ = FALSE; // This will be used to stop updating position
                                // when we come back from a delivery center

// Goal parameters
Direction goal_direction_;
int8_t current_goal_ = 0;
int8_t goal_row_ = 0;
int8_t goal_col_ = 0;

uint8_t deadend = 0;
uint8_t state_right = 0;
uint8_t state_left = 0;
uint8_t currentNoOfDots = 0;
uint8_t targetNodeIndex = 0;
uint8_t crossing_marker = 0;
uint8_t state = 0;

void StopRobot(void)
{
	set_motors(0, 0);
}

void StopRobotForever(void)
{
	StopRobot();
	while(1);
}

void run_1cm(void)
{
    set_m1_speed(30);
    set_m2_speed(30);
    delay_ms(100);
}

void turn180()
{	
    set_m1_speed(-50);
	set_m2_speed(50);
	delay_ms(600);
    while (TRUE)
    {
        uint16_t sensors[5];
        int ll = read_line(sensors, IR_EMITTERS_ON);
        if (sensors[LEFT_INNER_SENSOR] > 300)
        {
            break;
        }
        follow_line(ll);
    }
}

void solve_challenge(void)
{
    current_goal_ = get_first_goal();
    set_goal_data(current_goal_);
    uint8_t done = FALSE;
    initNodeLog();
    while(!done)
    {

        if (state_  == STATE_FOLLOW_LINE_UNTIL_CROSSING)
        {
            follow_line_until_crossing();   
        }
        else if (state_ == STATE_AT_CROSS)
        {
            int8_t delta_col_ = goal_col_ - current_col_;
            int8_t delta_row_ = goal_row_ - current_row_;
            // Can we minimize something by moving forward?
            if ((current_direction_ == DIRECTION_NORTH && delta_row_ > 0)
                || (current_direction_ == DIRECTION_SOUTH && delta_row_ < 0)
                || (current_direction_ == DIRECTION_WEST && delta_col_ > 0)
                || (current_direction_ == DIRECTION_EAST && delta_col_ < 0))
            {
                state_ = STATE_FOLLOW_LINE_UNTIL_CROSSING;
            }
            // North
            else if(current_direction_ == DIRECTION_NORTH && delta_col_ > 0)
            {
                rotate_ccw();
                state_ = STATE_FOLLOW_LINE_UNTIL_CROSSING;
            }
            else if(current_direction_ == DIRECTION_NORTH && delta_col_ < 0)
            {
                rotate_cw();
                state_ = STATE_FOLLOW_LINE_UNTIL_CROSSING;
            }
            // South
            else if(current_direction_ == DIRECTION_SOUTH && delta_col_ > 0)
            {
                rotate_cw();
                state_ = STATE_FOLLOW_LINE_UNTIL_CROSSING;
            }
            else if(current_direction_ == DIRECTION_SOUTH && delta_col_ < 0)
            {
                rotate_ccw();
                state_ = STATE_FOLLOW_LINE_UNTIL_CROSSING;
            }
            //East
            else if(current_direction_ == DIRECTION_EAST && delta_row_ > 0)
            {
                rotate_ccw();
                state_ = STATE_FOLLOW_LINE_UNTIL_CROSSING;
            }
            else if(current_direction_ == DIRECTION_EAST && delta_row_ < 0)
            {
                rotate_cw();
                state_ = STATE_FOLLOW_LINE_UNTIL_CROSSING;
            }
            //West
            else if(current_direction_ == DIRECTION_WEST && delta_row_ > 0)
            {
                rotate_cw();
                state_ = STATE_FOLLOW_LINE_UNTIL_CROSSING;
            }
            else if(current_direction_ == DIRECTION_WEST && delta_row_ < 0)
            {
                rotate_ccw();
                state_ = STATE_FOLLOW_LINE_UNTIL_CROSSING;
            }
            else
            {
                state_ = STATE_APPROACHING_CENTER;
            }

        }
        else if (state_ == STATE_APPROACHING_CENTER)
        {
            int8_t delta_direction = (int8_t) goal_direction_ - (int8_t) current_direction_;
            if (delta_direction == 0)
            {
                move_forward();
                state_ = STATE_ENTER_FIRST_GATE;
                //move_fovenkat(VENKAT_NO_ERR);
            }
            else
            {
                if (delta_direction == 1 || delta_direction == -3)
                {
                    rotate_cw();
                }
                else if( delta_direction == -1 || delta_direction == 3)
                {
                    rotate_ccw();
                }
                else
                {
                    venkat(VENKAT_INVALID_DELTA);
                }
            }
        }
        else if(state_ == STATE_ENTER_FIRST_GATE)
        {	
            out_on_delivery_ = TRUE;
            if (current_goal_ == 2 * (COL_COUNT + ROW_COUNT))
            {
                venkat_like_no_tomorrow();
            }
            else
            {
                follow_line_through_gate();
            }
            

        }
        else if (state_ == STATE_READING_CODE)
        {
            follow_line_read_code();
        }
        else if (state_ == STATE_RETURN_TO_ARENA)
        {
            follow_line_ignore_code();
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
        follow_line_narrow(sensors);
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
        if (out_on_delivery_)
        {
            out_on_delivery_ = FALSE;
        }
        else
        {
            update_pos_after_crossing();
        }

        state_ = STATE_AT_CROSS;
    }

    follow_line_narrow(sensors);
}

void follow_line_read_code(void)
{
    static int8_t right_bit = FALSE;
    static int8_t left_bit = FALSE;
    static int8_t bit_count = 0;

    uint16_t sensors[5];

    (void) read_line(sensors, IR_EMITTERS_ON);
    if (sensors[RIGHT_OUTHER_SENSOR] > 300 && !right_bit)
    {
        //Todo: Add comment
        bit_count++;
        // clear();
        // print_message_and_two_numbers("right", targetNodeIndex, currentNoOfDots);
        right_bit = TRUE;
    }
    else if (sensors[LEFT_OUTHER_SENSOR] > 300 && !left_bit)
    {
        uint8_t increment_value = 1 << bit_count;
        bit_count++;
        next_center_ ^= increment_value;
        // clear();
        // print_message_and_two_numbers("left", targetNodeIndex, currentNoOfDots);
        left_bit = TRUE;
    }
    else if (sensors[LEFT_OUTHER_SENSOR] < 300 && sensors[RIGHT_OUTHER_SENSOR] < 300 && (right_bit || left_bit))
    {
        right_bit = FALSE;
        left_bit = FALSE;
    }
    else if(sensors[LEFT_INNER_SENSOR] > 300 && sensors[RIGHT_INNER_SENSOR] > 300)
    {
        bit_count = 0;
        gate_state_++;
        addValueToNodeLog(next_center_, NORMAL);
        turn180();
        current_direction_ +=2;
        if (current_direction_ > DIRECTION_WEST ) 
        {
            current_direction_ -= 4;
        }
        state_ = STATE_RETURN_TO_ARENA;
        gate_state_ = GATE_STATE_NA; // until we do the bonus
    }
    follow_line_narrow(sensors);
}

void follow_line_through_gate(void)
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

    if (sensors[LEFT_INNER_SENSOR]  > 300 &&
    sensors[RIGHT_INNER_SENSOR] > 300)
    {
        if (!crossing_entered)
        {
            gate_state_++;
        }
        crossing_entered = TRUE;

    }

    if (crossing_entered &&
        (sensors[LEFT_INNER_SENSOR] < 100 ||
        sensors[RIGHT_INNER_SENSOR] < 100))  // || because the binary code might trip it
    {
        crossing_passed = TRUE;
        gate_state_++;
        if (gate_state_ == GATE_STATE_PASSED_FIRST_GATE)
        {
            state_= STATE_READING_CODE;
        }
    }

    follow_line(ll);
}

void follow_line_ignore_code(void)
{
    uint16_t sensors[5];
	static uint8_t crossing_entered = FALSE;
	static uint8_t crossing_passed = FALSE;


    int ll = read_line(sensors, IR_EMITTERS_ON);
    follow_line_narrow(sensors);

    if (crossing_entered && crossing_passed)
    {
        // We have already crossed, clear both
        crossing_entered = FALSE;
        crossing_passed = FALSE;
        state_ = STATE_FOLLOW_LINE_UNTIL_CROSSING;
        current_goal_ = next_center_;
        next_center_ = 0;
        print_goal();
        set_goal_data(current_goal_);
        follow_line(ll);
        return;
    }

    if (sensors[LEFT_INNER_SENSOR]  > 300 &&
    sensors[RIGHT_INNER_SENSOR] > 300)
    {
        crossing_entered = TRUE;
    }

    if (crossing_entered &&
        (sensors[LEFT_INNER_SENSOR] < 300 ||
        sensors[RIGHT_INNER_SENSOR] < 300))  // || because the binary code might trip it
    {
        crossing_passed = TRUE;
    }

}

uint8_t check_end(uint8_t row, uint8_t col)
{
    return col == COL_COUNT;
}

uint8_t get_first_goal()
{
    return COL_COUNT + 1;
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

    // clear();
    // lcd_goto_xy(0,0);
    // print("Vnkt");
    // lcd_goto_xy(0,1);
    // print("cw");
    uint8_t first_time = TRUE;
    while(!done)
    {
        (void) read_line(sensors, IR_EMITTERS_ON);
        l = ((uint32_t)sensors[2] * 2000UL +
			 (uint32_t)sensors[3] * 3000UL +
			 (uint32_t)sensors[4] * 4000UL) / (sensors[2] + sensors[3] + sensors[4]);
        follow_line(l);
        if (first_time)
        {
            delay_ms(100);
            first_time = FALSE;
        }

        (void) read_line(sensors, IR_EMITTERS_ON);
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
    return;
}

void rotate_ccw()
{
    uint16_t sensors[5];
	uint16_t l;
    uint8_t done = 0;

    // clear();
    // lcd_goto_xy(0,0);
    // print("Vnkt");
    // lcd_goto_xy(0,1);
    // print("ccw");
    uint8_t first_time = TRUE;
    while(!done)
    {
        (void) read_line(sensors, IR_EMITTERS_ON);
        l = ((uint32_t)sensors[0] * 0000UL +
			 (uint32_t)sensors[1] * 1000UL +
			 (uint32_t)sensors[2] * 2000UL) / (sensors[0] + sensors[1] + sensors[2]);
        follow_line(l);
        if (first_time)
        {
            delay_ms(100);
            first_time = FALSE;
        }
        (void) read_line(sensors, IR_EMITTERS_ON);
        if (sensors[LEFT_OUTHER_SENSOR] < 100 && sensors[RIGHT_OUTHER_SENSOR] < 100)
        {
            done = TRUE;
            if (current_direction_ == DIRECTION_NORTH)
            {
                current_direction_ = DIRECTION_WEST;
            }
            else
            {
                current_direction_--;
            }
        }
    }
    return;
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
        int8_t delta = goal_col_ - current_col_;
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

void rotate_to_goal(void)
{
    int8_t delta = (int8_t) goal_direction_ - (int8_t) current_direction_;
    if (delta == 3) delta = -1;
    if (delta == -3) delta = 1;

    if (delta == 1)
    {
        rotate_cw();
    }
    else if (delta == -1)
    {
        rotate_ccw();
    }
    else
    {
        venkat(VENKAT_INVALID_DELTA+ 10 + current_direction_);
    }
}

void print_goal(void)
{
    clear();
    lcd_goto_xy(0,0);
    print_long(goal_row_);
    print(":");
    print_long(goal_col_);
    lcd_goto_xy(0,1);
    print_long(current_goal_);

}

void move_forward()
{
    uint16_t sensors[5];
    int ll = read_line(sensors, IR_EMITTERS_ON);
    for (int8_t ii = 0; ii < 25; ii++)
    {
        follow_line(ll);
        ll = read_line(sensors, IR_EMITTERS_ON);
    }

}

void venkat_like_no_tomorrow(void)
{
    uint16_t sensors[5];
    while(TRUE)
    {
        int ll = read_line(sensors, IR_EMITTERS_ON);
        if (sensors[MID_SENSOR]< 300 && sensors[RIGHT_INNER_SENSOR] < 300) break;
        follow_line(ll);
    }
    play_sound(0);
    lcd_goto_xy(0,0);
    print("Vnkt");
    print_long(goal_col_);
    lcd_goto_xy(0,1);
    print("was here!");
    play_sound(2);
    stop();
    printVisitedNodes();
    venkat(VENKAT_HAS_DONE_IT);

}