
#ifndef TSM_H_
#define TSM_H_

#include <stdint.h>

typedef enum
{
	DIRECTION_NORTH,
	DIRECTION_EAST,
	DIRECTION_SOUTH,
    DIRECTION_WEST,
} Direction;

typedef enum
{
    STATE_FOLLOW_LINE_UNTIL_CROSSING,
    STATE_AT_CROSS,
    STATE_APPROACHING_CENTER,
    STATE_ENTER_FIRST_GATE,
    STATE_READING_CODE,
    STATE_RETURN_TO_ARENA,

} State;

typedef enum
{
    GATE_STATE_NA,
    GATE_STATE_ENTERED_FIRST_GATE,
    GATE_STATE_PASSED_FIRST_GATE,
    GATE_STATE_ENTERED_SECOND_GATE,
    GATE_STATE_PASSED_SECOND_GATE,
    GATE_STATE_ENTERED_THIRD_GATE,
    GATE_STATE_PASSED_THIRD_GATE,
} Gate_state;

typedef enum
{
    VENKAT_NO_ERR,                          // 0
    VENKAT_INVALID_CROSSING_DIRECTION,      // 1
    VENKAT_INVALID_CENTER,                  // 2
    VENKAT_INVALID_GOAL_DIRECTION,          // 3
    VENKAT_INVALID_DELTA,                   // 4
    VENKAT_OUTSIDE_ARENA                    // 5

} Venkat_type;

void follow_line_until_crossing(void);
void update_pos_after_crossing(void);
uint8_t check_end(uint8_t row, uint8_t col);
uint8_t get_first_goal(void);
void set_goal_data(uint8_t center);
void solve_challenge(void);
void venkat(Venkat_type err);
void print_pos(void);
void rotate_cw(void);
void rotate_ccw(void);
uint8_t check_row_col(void); // Is the robot in the correct row (for row centers),
                          // or correct col (for col centers)
void approach_goal_col_row(void);
void approach_goal_col_row2(void);
void sanity_check(void);
void rotate_to_goal(void);
void print_goal();
void move_forward(); // Helper function to move the robot a couple of cm's forward
void follow_line_through_gate(void);
void follow_line_read_code(void);
void turn180(void);
void follow_line_ignore_code(void);



#endif /* TSM_H_ */