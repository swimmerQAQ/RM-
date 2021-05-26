#include "PID.h"
#include "stdint.h"
void no_fast(void)
{
	for(int i=0;i<4;i++)
	{
		if(pwm[i]>950)
		{
			pwm[i]=950;
		}
	}
}
void xunji_set_action(int x,int y,int z)
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
 int PID(int goal,int encoder)
{
	double propotion=100,integration=0.1,differential=500;
	static int bias_now,bias_past,bias_sum_past,pwm_out;
	bias_now = encoder - goal;
	bias_sum_past+=bias_now;
	pwm_out = propotion*bias_now + integration*bias_sum_past + differential*(bias_past-bias_now);
	bias_past = bias_now;
	return pwm_out;
}
int PID_increment(double goal,double current)
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
