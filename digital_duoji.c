#include "digital_duoji.h"

int duoji[3]={1500,1500,1400};
void duoji_motor(int key)
{
	switch(key)
	{
		/*
		case 9:->10
			duoji[0]=duoji[0]-25;
			break;
		case 11:->12
			duoji[0]=duoji[0]+25;
			break;
		case 10:->11
			duoji[1]=duoji[1]-25;
			break;
		case 12:->//9
			duoji[1]=duoji[1]+25;
		*/
		case 12:
			duoji[0]=duoji[0]-25;
			break;
		case 10:
			duoji[0]=duoji[0]+25;
			break;
		case 11:
			duoji[1]=duoji[1]-25;
			break;
		case 9:
			duoji[1]=duoji[1]+25;
			break;
		case 2:
			duoji[2]=duoji[2]+25;
		break;
		case 3:
			duoji[2]=duoji[2]-25;
		break;
		default:
			break;
	}
}

void duoji_constrain(void)
{
		if(duoji[0]>1800)
		{
			duoji[0]=1800;
		}
		if(duoji[1]>1500)
		{
			duoji[1]=1500;
		}
		if(duoji[0]<700)
		{
			duoji[0]=700;
		}
		if(duoji[1]<900)
		{
			duoji[1]=900;
		}
		if(duoji[2]>1725)
		{
			duoji[2]=1725;
		}
		if(duoji[2]<1400)
		{
			duoji[2]=1400;
		}
		
}



















