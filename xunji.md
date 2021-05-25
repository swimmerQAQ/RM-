```c
#include "xunji.h" 
#include "main.h"
int rec[10]={1,1,1,1,1,1,1,1,1,1};//前后共十路循迹
int xunji_read[2]={2,2};//循迹所需的标志位
int flag_x=0;//x轴方向bias偏差
int flag_w=0;//z轴方向bias偏差
int bulf = 0;//缓存
int sum=0;//求和
int data[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//模仿ps2手柄里面的数据，没有全用到
extern uint8_t keep;//全局区变量，用于限制函数的使用次数
/*

rec_0 1 2 3 4

小车的循迹模型

rec_5 6 7 8 9

*/
void read_pin(void)
{
	rec[0] = HAL_GPIO_ReadPin(R0_GPIO_Port,R0_Pin);
	rec[1] = HAL_GPIO_ReadPin(R1_GPIO_Port,R1_Pin);
	rec[2] = HAL_GPIO_ReadPin(R2_GPIO_Port,R2_Pin);
	rec[3] = HAL_GPIO_ReadPin(R3_GPIO_Port,R3_Pin);
	rec[4] = HAL_GPIO_ReadPin(R4_GPIO_Port,R4_Pin);
	rec[5] = HAL_GPIO_ReadPin(R5_GPIO_Port,R5_Pin);
	rec[6] = HAL_GPIO_ReadPin(R6_GPIO_Port,R6_Pin);
	rec[7] = HAL_GPIO_ReadPin(R7_GPIO_Port,R7_Pin);
	rec[8] = HAL_GPIO_ReadPin(R8_GPIO_Port,R8_Pin);
	rec[9] = HAL_GPIO_ReadPin(R9_GPIO_Port,R9_Pin);
}
void readline(int rec[10])//根据循迹模块读取到的高低电平（激光）对前后的标志位进行赋值，有单个详细描述
{
	//external priority
	read_pin();
	for(int i=2;i>-1;i--)//get( 0/1/2 )
	{
		if(rec[i]==0)//front_2_>1_>0
		{
			xunji_read[1]=2-i;
		}
		if(rec[i+5]==0)//rear_7_>6_>5
		{
			xunji_read[0]=2-i;
		}
	}
	for(int i=3;i<5;i++)//get( 0/-1/-2 )
	{
		if(rec[i]==0)//front_2_>3_>4
		{
			xunji_read[1]=2-i;
		}
		if(rec[i+5]==0)//rear_7_>8_>9
		{
			xunji_read[0]=2-i;
		}
	}
}
void check_xunji(void)//检测是否出现全白的情况，通过保留的标志位保持一直自旋，直到循迹模块读到另一边
{
	 read_pin();
		sum=rec[0]+rec[1]+rec[2]+rec[3]+rec[4];
	
				if(rec[0]==0 || rec[4]==0)
			{
				keep=0;
			}
			
		if(sum==5 && keep==0)
		{
				keep=11;
				if(flag_w>0)
			{
				/* w>0 clockwise w<128 */
				data[5]=15;
				data[8]=127;
				data[7]=127;
			}
			if(flag_w<0)
			{
				/* w<0 anticlockwise w>128 */
				data[5]=240;
				data[8]=127;
				data[7]=127;
			}

		}
//////////////////////////////
}
/*
PSS_RX 5                //ÓÒÒ¡¸ËXÖáÊý¾Ý
PSS_RY 6
PSS_LX 7
PSS_LY 8
*/
/*以下方法属于前后读取数据，形成向量来模拟，小车遇到的白线情况*/
/*请在模块比较灵敏的情况下使用，该算法拟合效果使得macanum轮的优势发挥，但易受到干扰！！！*/
/*前后分别读取，左边依次可能为210，到右边-1，-2*/
/*  即 xunji【1】和xunji【0】得到的数是{2，1，0，-1，-2}*/
/*对于xunji【1】-xunji【0】：所形成的差值，易证，刚好表示的是z轴的bias，因为y轴不变，x轴向上相差越大，z的偏离越大*/
/*同理*/
/*xunji【1】+xunji【0】刚好表示的是x轴向上的bias对其正负号表达的是左和右*/
/*最后，将pwm和正反转信号传给motor就可以了*/
void xunji_motor(void)
{
	flag_w = xunji_read[1] - xunji_read[0];//from +4 to -4
	flag_x = xunji_read[1] + xunji_read[0];
	if(flag_x==0)
	{
		/* x=0 none x_v */
			data[7]=128;
	}else if(flag_x>0)
	{
		/* x>0 left x_v <128 */
		bulf = flag_x+1;
		data[7]=128-bulf*20;
		bulf=0;
	}else
	{
		/* x<0 right x_v >128 */
		bulf = -flag_x+1;
		data[7]=128+bulf*20;
		bulf=0;
	}
	if(flag_w==0)
	{
		/* w=0 */
		data[5]=128;
	}else if(flag_w>0)
	{
		/* w>0 clockwise w<128 */
		bulf=flag_w+1;
		data[5]=128-15*bulf;
	}else
	{
		/* w<0 anticlockwise w>128 */
		bulf = -flag_w+1;
		data[5]=128+5*bulf;
	}
	data[8]=30;


}
```

```c
#ifndef __XUNJI_H
#define __XUNJI_H

void readline(int rec[10]);
void read_pin(void);
void xunji_motor(void);
void check_xunji(void);






#endif

```

