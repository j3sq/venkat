
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
    STATE_LOOKING_FOR_BONUS_CODE,
    STATE_READING_BONUS,
    STATE_RETURN_TO_ARENA,
    STATE_RETURN_TO_ARENA_FROM_BONUS,

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
    GATE_STATE_NO_BONUS,
} Gate_state;

typedef enum
{
    VENKAT_NO_ERR,                          // 0
    VENKAT_INVALID_CROSSING_DIRECTION,      // 1
    VENKAT_INVALID_CENTER,                  // 2
    VENKAT_INVALID_GOAL_DIRECTION,          // 3
    VENKAT_INVALID_DELTA,                   // 4
    VENKAT_OUTSIDE_ARENA,                   // 5
    VENKAT_INVALID_BONUS_DIRECTION,         // 6
    VENKAT_HAS_DONE_IT,                     // 7

} Venkat_type;

typedef struct
{
    int8_t col;
    int8_t row;
}  Crossing;

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
void print_goal(void);
void move_forward(); // Helper function to move the robot a couple of cm's forward
void follow_line_through_gate(void);
void follow_line_read_code(void);
void turn180(void);
void follow_line_ignore_code(void);
void venkat_like_no_tomorrow(void);

void set_bonus_locations();
void follow_line_read_bonus();
void follow_line_check_bonus_or_not();
void follow_line_ignore_code_and_bonus();
void print_bonus();
void set_bonus_direction(uint8_t bonus_idx);


#endif /* TSM_H_ */