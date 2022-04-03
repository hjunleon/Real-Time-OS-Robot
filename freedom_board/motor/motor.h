# include <MKL25Z4.h>
#ifndef MOTOR_H
#define MOTOR_H
void initMotor(void);
void left_motor(void);
void right_motor(void);
void stop(void);
void disable_motor(void);
typedef enum{
	FORWARD,
	BACKWARD,
	STOP,
	LEFT,
	RIGHT,
	STRAIGHT
}dir;


typedef struct{
	dir direction;
	uint8_t level;
} motor_cmd;
void set_motors(motor_cmd cmd);
void calc_left(uint8_t level);
void calc_left_back(uint8_t level);
void calc_right(uint8_t level);
void calc_right_back(uint8_t level);
motor_cmd create_motor_cmd(uint8_t level, dir direction);

unsigned int get_move_state();

extern volatile dir f_b_dir;
extern volatile dir l_r_dir;
#endif
