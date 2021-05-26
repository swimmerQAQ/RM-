#ifndef __MOTOR_H
#define __MOTOR_H
#include "stdint.h"
	#define speed_AF speed[0]
	#define speed_BF speed[1]
	#define speed_BR speed[2]
	#define speed_AR speed[3]
void AF_stop(void);
void AF_forth(void);
void AF_back(void);
void BF_back(void);
void BF_forth(void);
void BF_stop(void);
void AR_back(void);
void AR_forth(void);
void AR_stop(void);
void BR_back(void);
void BR_forth(void);
void BR_stop(void);

void motor(uint8_t which_wheel_xuan[4]);


void encoder_restrict(void);
#endif


