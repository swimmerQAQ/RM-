#include "xunji.h" 
#include "main.h"
int rec[10]={1,1,1,1,1,1,1,1,1,1};
int xunji_read[2]={2,2};
int flag_x=0;
int flag_w=0;
int bulf = 0;
int sum=0;
int data[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
extern uint8_t keep;
/*

rec_0 1 2 3 4

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
void readline(int rec[10])
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
void check_xunji(void)
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











