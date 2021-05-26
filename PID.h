#ifndef __PID_H
#define __PID_H
#include "stdint.h"
#define La 1.4
#define Lb 1.85
void get_x_y_raw(uint16_t Vaf,uint16_t Vbf,uint16_t Vbr,uint16_t Var);
void set_bit(void);

void set_action(int x,int y,int z);
void high_set_action(int x,int y,int z);
void xunji_set_action(int x,int y,int z);

int PID(int goal,int encoder);
int PID_increment(double goal, double current);

extern int pwm[4];//set_pwm
extern uint8_t Mecanum[4];
extern double	check[3];

void no_fast(void);
#endif
