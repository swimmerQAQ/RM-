PID  &  pwm_calculate

```c
#include "PID.h"
#include "stdint.h"
void no_fast(void)//限幅pwm，防止烧坏电机
{
	for(int i=0;i<4;i++)
	{
		if(pwm[i]>950)
		{
			pwm[i]=950;
		}
	}
}
void xunji_set_action(int x,int y,int z)//设置根据获得的xyz三轴的数据，运用运动学结算计算得到四路pwm的大小，同时我分为低速档，高速档以及循迹模式挡位三个。
{
		x=(127-x);
		y=(127-y)*2;
		z=z-127;
		const int value = (La+Lb)/2;
		z = z*value*4;
		pwm[0]=y-x+z;
		pwm[1]=y+x-z;
		pwm[2]=y-x-z;
		pwm[3]=y+x+z;
}
void high_set_action(int x,int y,int z)
{
		x=(128-x)*7;
		y=(128-y)*7;
		z=z-128;
		const int value = (La+Lb)/2;
		z = z*value*3;
		pwm[0]=y-x+z;
		pwm[1]=y+x-z;
		pwm[2]=y-x-z;
		pwm[3]=y+x+z;
}
void set_action(int x,int y,int z)
 {
	 /* pwm_0_>AF pwm_1_>BF pwm_2_>BR pwm_3_>AR */
	 //mecanum 0-> ar/1->br/2->af/3->bf
	 //check on 2021.4.29
	 //xyz_>>>>porpotion
    //对应的pwm分别为左前，右前，左后，右后
	 x=(127-x)*2;
	 y=(127-y)*2;
	 z=z-127;
	 const int value = (La+Lb)/2;
	 z = z*value*2;
	 pwm[0]=y-x+z;
	 pwm[1]=y+x-z;
	 pwm[2]=y-x-z;
	 pwm[3]=y+x+z;
	 
 }
 void set_bit(void)//remote control
     //对每个计算得到的pwm值进行处理，并且对于负值我们可以通过另外一个标志位得到反向的记号，同时将pwm取反送给定时器。
 {
	 for(int i=0;i<4;i++)
	 if(pwm[i]<0)
	 {
		 pwm[i]=-1*pwm[i];
		 Mecanum[i]=2;
	 }else if(pwm[i]==0)
	 {
		 Mecanum[i]=0;
	 }else
	 {
		 Mecanum[i]=1;
	 }
 }
 int PID(int goal,int encoder)//位置式pid器，一步到位，伤电机
{
	double propotion=100,integration=0.1,differential=500;
	static int bias_now,bias_past,bias_sum_past,pwm_out;
	bias_now = encoder - goal;
	bias_sum_past+=bias_now;
	pwm_out = propotion*bias_now + integration*bias_sum_past + differential*(bias_past-bias_now);
	bias_past = bias_now;
	return pwm_out;
}
int PID_increment(double goal,double current)//增量式pid控制，对电机更友好，容错率更高，但是相对反应慢一些些
{
	double propotion=120,integration=0.1,differential=500;
	static int bias_c,bias_past,bias_ppast,out;
	bias_c = goal - current;
	out= propotion * bias_c +
			 integration * bias_past +
				differential * (bias_c - 2*bias_past +bias_ppast);
	bias_ppast = bias_past;
	bias_past = bias_c;
	return out;
}
```

```c
#ifndef __PID_H
#define __PID_H
#include "stdint.h"
#define La 1.4//这两个是做运动学结算所需要的车身常量
#define Lb 1.85//
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
```

