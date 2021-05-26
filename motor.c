#include "motor.h"
#include "main.h"
#include "stdio.h"

/*
	Mecanum[0] Mecanum[1] Mecanum[2] Mecanum[3]
//AF2 AF1////BF1 BF2////BR1 BR2////AR1 AR2///
//STBY_>PE1;
//1_>forth 2_>rear 0_>stop */
/* pwm_0_>AF pwm_1_>BF pwm_2_>BR pwm_3_>AR */
/*logic check*/
//mecanum 0-> ar/1->br/2->af/3->bf
extern int speed[4];
void AF_stop(void)
{
	//00
	HAL_GPIO_WritePin(AF1_GPIO_Port, AF1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(AF2_GPIO_Port, AF2_Pin, GPIO_PIN_RESET);
}
void AF_forth(void)
{
	//01
	HAL_GPIO_WritePin(AF1_GPIO_Port, AF1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(AF2_GPIO_Port, AF2_Pin, GPIO_PIN_SET);
}
void AF_back(void)
{
	//10
	HAL_GPIO_WritePin(AF1_GPIO_Port, AF1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(AF2_GPIO_Port, AF2_Pin, GPIO_PIN_RESET);
}
///////////////////////////////////////////////////////////////////////
void BF_back(void)
{
	//10
	HAL_GPIO_WritePin(BF1_GPIO_Port, BF1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(BF2_GPIO_Port, BF2_Pin, GPIO_PIN_RESET);
}
void BF_forth(void)
{
	//01
	HAL_GPIO_WritePin(BF1_GPIO_Port, BF1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(BF2_GPIO_Port, BF2_Pin, GPIO_PIN_SET);
}
void BF_stop(void)
{
	//00
	HAL_GPIO_WritePin(BF1_GPIO_Port, BF1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(BF2_GPIO_Port, BF2_Pin, GPIO_PIN_RESET);
}
////////////////////////////////////////////////////////////////////////////
void AR_back(void)
{
	//10
	HAL_GPIO_WritePin(AR1_GPIO_Port, AR1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(AR2_GPIO_Port, AR2_Pin, GPIO_PIN_RESET);
}
void AR_forth(void)
{
	//01
	HAL_GPIO_WritePin(AR1_GPIO_Port, AR1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(AR2_GPIO_Port, AR2_Pin, GPIO_PIN_SET);
}
void AR_stop(void)
{
	//00
	HAL_GPIO_WritePin(AR1_GPIO_Port, AR1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(AR2_GPIO_Port, AR2_Pin, GPIO_PIN_RESET);
}
/////////////////////////////////////////////////////////////////////////
void BR_back(void)
{
	//10
	HAL_GPIO_WritePin(BR1_GPIO_Port, BR1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(BR2_GPIO_Port, BR2_Pin, GPIO_PIN_RESET);
}
void BR_forth(void)
{
	//01
	HAL_GPIO_WritePin(BR1_GPIO_Port, BR1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(BR2_GPIO_Port, BR2_Pin, GPIO_PIN_SET);
}
void BR_stop(void)
{
	//00
	HAL_GPIO_WritePin(BR1_GPIO_Port, BR1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(BR2_GPIO_Port, BR2_Pin, GPIO_PIN_RESET);
}
//0_af/1_bf/2_br/3_ar
 void motor(uint8_t which_wheel_xuan[4])//mecanum 0-> ar/1->br/2->af/3->bf
 {
		switch(which_wheel_xuan[0])
	 {
		 case 0://stop
			 AF_stop();
		 break;
		 case 1://forth
			 AF_back();
		 break;
		 case 2://back
			 AF_forth();
		 break;
		 default : printf("error in the switch_wheel_AF");
	 }

		switch(which_wheel_xuan[1])
	 {
		 case 0://stop
			 BF_stop();
		 break;
		 case 1://forth
			 BF_back();
		 break;
		 case 2://back
			 BF_forth();
		 break;
		 default : printf("error in the switch_wheel_BF");
	 }

	 switch(which_wheel_xuan[2])
	{
	 case 0://stop
		 BR_stop();
	 break;
	 case 1://forth
		 BR_forth();
	 break;
	 case 2://back
		 BR_back();
	 break;
	 default : printf("error in the switch_wheel_BR");
	}

	 switch(which_wheel_xuan[3])
	{
	 case 0://stop
		 AR_stop();
	 break;
	 case 1://forth
		 AR_forth();
	 break;
	 case 2://back
		 AR_back();
	 break;
	 default : printf("error in the switch_wheel_AR");
	}
	} 
 void encoder_restrict(void)
 {
	 if(speed_AF>30000)
		{
			speed_AF=speed_AF-65535;
		}
		if(speed_BF>30000)
		{
			speed_BF=speed_BF-65535;
		}
		if(speed_AR>30000)
		{
			speed_AR=speed_AR-65535;
		}
		if(speed_BR>30000)
		{
			speed_BR=speed_BR-65535;
		}
 }




