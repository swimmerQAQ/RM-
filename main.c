/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "PS2.h"
#include "dwt_stm32_delay.h"
#include "motor.h"
#include "PID.h"
#include "digital_duoji.h"
#include "xunji.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
	uint8_t Mecanum[4]={1,1,1,1};
	int speed[4];
	double	check[3]={0,0,0};///encoder test//0_Vx//1_Vy//2_omg
	int pwm[4]={0,0,0,0};//set_pwm
			uint8_t key=0;

	//xunji
	uint8_t keep=1;
	uint8_t keep_one_catch = 1;
extern int rec[10];
extern int xunji_read[2];
	//
	extern int duoji[3];
	int catch_flag=1;
	//usart1_tof
	uint8_t hello[] = " hello,let'go now ";
	uint8_t receive [13]={0};
	int dist;//for catch
	//
extern uint16_t Handkey;
extern uint8_t Comd[2];	//开始命令。请求数据
extern uint8_t Data[9]; //数据存储数组
extern uint16_t MASK[16];	//按键值与按键明
	extern int data[16];//xunji_data
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim8;
TIM_HandleTypeDef htim9;
TIM_HandleTypeDef htim13;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM5_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM8_Init(void);
static void MX_TIM9_Init(void);
static void MX_TIM13_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int fputc(int c, FILE *stream)
 {
	 HAL_UART_Transmit(&huart1 , (uint8_t *)&c, 1, 0xFFFF);
   return c;
 }
uint8_t Buffer;//usart1
 void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART1)
    {
        HAL_UART_Transmit_IT(&huart1, (uint8_t *)receive, 13);
        HAL_UART_Receive_IT(&huart1,(uint8_t *)receive,13);
				dist=(int)receive;
				printf(" dist = %d ",dist);
    }
}

 void get_x_y_raw(uint16_t Vaf,uint16_t Vbf,uint16_t Vbr,uint16_t Var)
 {
	 /* AF_>1 BF_>2 AR_4 BR_3 get_current_position */
	 double vx;
	 double vy;
	 double omg;
	 vx=(Vbf-Vaf+Var-Vbr)/4;
	 check[0]=vx;
	 vy=(Vbf+Vaf+Var+Vbr)/4;
	 check[1]=vy;
	 omg=(Vaf-Vbr+Var-Vbf)/4/La/Lb;
	 check[2]=omg;
 }
 void motor_box(void)
 {
	 set_bit();//need pwm[x]//->//Mecanum[x]//
			motor(Mecanum);//-> F_&_B //
			no_fast();//restrict_pwm//
			__HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,pwm[1]);////BF
			__HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_2,pwm[2]);////BR
			__HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_3,pwm[0]);////AF
			__HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_4,pwm[3]);////AR
 }
 void duoji_execute(void)
 {
	 duoji_constrain();
	 __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_4,duoji[0]);//up_down
	 __HAL_TIM_SetCompare(&htim9,TIM_CHANNEL_1,duoji[1]);//forth_back
	 __HAL_TIM_SetCompare(&htim9,TIM_CHANNEL_2,duoji[2]);//catch
 }
 void catch_first(void)
 {
	 HAL_GPIO_WritePin(STBY_GPIO_Port,STBY_Pin,GPIO_PIN_RESET);
	 while(duoji[0]>1200)
	 {
		 if(duoji[0]>1200)
		 {
			 duoji[0]=duoji[0]--;
		 }
		 __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_4,duoji[0]);//up
		 HAL_Delay(3);
	 }
	 	 while(duoji[1]>1075 || duoji[0]<1625)
	 {
		 if(duoji[1]>1025)
		 {
			 duoji[1]=duoji[1]--;
		 }
		 if(duoji[0]<1650)
		 {
			 duoji[0]=duoji[0]++;
		 }
			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_4,duoji[0]);//down
			__HAL_TIM_SetCompare(&htim9,TIM_CHANNEL_1,duoji[1]);//forth
		  HAL_Delay(3);
	 }
 }
 void catch_second(void)
 {
	 HAL_GPIO_WritePin(STBY_GPIO_Port,STBY_Pin,GPIO_PIN_RESET);
	 while(duoji[2]<1700)
	 {
		if(duoji[2]<1700)
		{
			duoji[2]++;
		}
		__HAL_TIM_SetCompare(&htim9,TIM_CHANNEL_2,duoji[2]);//catch
		HAL_Delay(3);
		
	 }
//	 duoji[2]=1750;
//	 __HAL_TIM_SetCompare(&htim9,TIM_CHANNEL_2,duoji[2]);//catch
//	 HAL_Delay(1000);
	 
	 while(duoji[1]<1500 || duoji[0]>1200)
	 {
		 if(duoji[1]<1500)
		 {
			 duoji[1]=duoji[1]++;
		 }
		 if(duoji[0]>1200)
		 {
			 duoji[0]=duoji[0]--;
		 }
			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_4,duoji[0]);//down
			__HAL_TIM_SetCompare(&htim9,TIM_CHANNEL_1,duoji[1]);//forth
		 HAL_Delay(3);
	 }
	 while(duoji[0]<1500)
	 {
		 if(duoji[0]<1500)
		 {
			 duoji[0]=duoji[0]++;
		 }
			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_4,duoji[0]);//down
			HAL_Delay(3);
	 }
 }
 void catch_one(void)
 {
	 HAL_GPIO_WritePin(STBY_GPIO_Port,STBY_Pin,GPIO_PIN_RESET);
	 while(duoji[2]>1400)
	 {
		if(duoji[2]>1400)
		{
			duoji[2]--;
		}
		__HAL_TIM_SetCompare(&htim9,TIM_CHANNEL_2,duoji[2]);//catch
		HAL_Delay(3);
		
	 }
	 catch_second();

 }
	 
//	 while(duoji[0]>1200)
//	 {
//		 if(duoji[0]>1200)
//		 {
//			 duoji[0]=duoji[0]--;
//		 }
//		 __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_4,duoji[0]);//up
//		 HAL_Delay(3);
//	 }
//	 	 while(duoji[1]>1025 || duoji[0]<1650)
//	 {
//		 if(duoji[1]>1025)
//		 {
//			 duoji[1]=duoji[1]--;
//		 }
//		 if(duoji[0]<1650)
//		 {
//			 duoji[0]=duoji[0]++;
//		 }
//			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_4,duoji[0]);//down
//			__HAL_TIM_SetCompare(&htim9,TIM_CHANNEL_1,duoji[1]);//forth
//		  HAL_Delay(3);
//	 }
//	 duoji[2]=1725;
//	 __HAL_TIM_SetCompare(&htim9,TIM_CHANNEL_2,duoji[2]);//catch
//	 HAL_Delay(1000);
//	 
//	 while(duoji[1]<1500 || duoji[0]>1200)
//	 {
//		 if(duoji[1]<1500)
//		 {
//			 duoji[1]=duoji[1]++;
//		 }
//		 if(duoji[0]>1200)
//		 {
//			 duoji[0]=duoji[0]--;
//		 }
//			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_4,duoji[0]);//down
//			__HAL_TIM_SetCompare(&htim9,TIM_CHANNEL_1,duoji[1]);//forth
//		 HAL_Delay(3);
//	 }
//	 while(duoji[0]<1500)
//	 {
//		 if(duoji[0]<1500)
//		 {
//			 duoji[0]=duoji[0]++;
//		 }
//			__HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_4,duoji[0]);//down
//			HAL_Delay(3);
//	 }
//	 /*
//	 while(duoji[1]<1500)
//	 {
//		 if(duoji[1]<1500)
//		 {
//			 duoji[1]=duoji[0]++;
//		 }

//	 }
	 /*
	 duoji[1]=1250;
	 __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_4,duoji[1]);//up
	 HAL_Delay(1000);
	 
	 duoji[0]=1250;
	 __HAL_TIM_SetCompare(&htim9,TIM_CHANNEL_1,duoji[0]);//forth
	 HAL_Delay(1000);
	 
	 duoji[1]=1500;
	 __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_4,duoji[1]);//down
	 HAL_Delay(1000);
	 
	 duoji[2]=1725;
	 __HAL_TIM_SetCompare(&htim9,TIM_CHANNEL_2,duoji[2]);//catch
	 HAL_Delay(1000);

	 duoji[1]=1250;
	 __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_4,duoji[1]);//up
	 HAL_Delay(1000);
	 
	 duoji[0]=1500;
	 __HAL_TIM_SetCompare(&htim9,TIM_CHANNEL_1,duoji[0]);//back
	 HAL_Delay(1000);
	 
	 duoji[1]=1500;
	 __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_4,duoji[1]);//down
	 HAL_Delay(1000);*/
	 
 
// void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
// {
//	 if (htim == (&htim13))
//    {
//			speed_AF=(uint32_t)(__HAL_TIM_GET_COUNTER(&htim3));
//			speed_AR=(uint32_t)(__HAL_TIM_GET_COUNTER(&htim2));
//			speed_BF=(uint32_t)(__HAL_TIM_GET_COUNTER(&htim1));
//			speed_BR=(uint32_t)(__HAL_TIM_GET_COUNTER(&htim5));
//			encoder_restrict();
//			get_x_y_raw(speed_AF,speed_BF,speed_BR,speed_AR);
//			printf(" speed_AF = %d ",speed_AF);
//			printf("present vx=%lf,vy=%lf,omg=%lf \r\n",check[0],check[1],check[2]);//current_current_current_speed!!!
//			__HAL_TIM_SET_COUNTER(&htim1,0);
//			__HAL_TIM_SET_COUNTER(&htim2,0);
//			__HAL_TIM_SET_COUNTER(&htim3,0);
//			__HAL_TIM_SET_COUNTER(&htim5,0);
//    }
// }
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	
  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
	if(DWT_Delay_Init())
  {
    Error_Handler(); /* Call Error Handler */
  }
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_USART1_UART_Init();
  MX_TIM8_Init();
  MX_TIM9_Init();
  MX_TIM13_Init();
  /* USER CODE BEGIN 2 */
	PS2_SetInit();
	
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_4);
	
	HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&htim9,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim9,TIM_CHANNEL_2);
	
	//HAL_TIM_Base_Start_IT(&htim13);
	//HAL_TIM_PWM_Start(&htim13,TIM_CHANNEL_1);
	
//	HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
//	HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
//	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
//	HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_ALL);
	int cnt=1;
	printf(" Inition has finished \r\n");
	HAL_UART_Transmit_IT(&huart1,(uint8_t *)hello,sizeof(hello));//out put
	HAL_UART_Receive_IT(&huart1,(uint8_t *)receive,13);//receive data
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		
		HAL_Delay(50);
		key=PS2_DataKey();
		
		if(cnt==1500)
		{
		readline(rec);
		dist=rec[0]+rec[1]+rec[2]+rec[3]+rec[4];
		if(keep==1 && dist==0)
			{
				data[5]=127;
				data[7]=127;
				data[8]=60;
				HAL_GPIO_WritePin(STBY_GPIO_Port,STBY_Pin,GPIO_PIN_SET);
				xunji_set_action(data[7],data[8],data[5]);//should x_y_omg -> four pwm
				motor_box();
				HAL_Delay(600);
				data[5]=185;
				data[7]=60;
				data[8]=127;
				HAL_GPIO_WritePin(STBY_GPIO_Port,STBY_Pin,GPIO_PIN_SET);
				xunji_set_action(data[7],data[8],data[5]);//should x_y_omg -> four pwm
				motor_box();
				HAL_Delay(600);
				keep--;
				catch_first();
			}
			if(rec[5]==0)
			{
				catch_flag++;
			}
			if(catch_flag==2)
			{
				catch_second();
				catch_flag=3;
			}
		}
		
		switch(key)
		{
			case 13:
			keep=1;
			cnt=666;//low speed
			break;
			
			case 14:
			cnt=1500;//xunji
			break;
			
			case 15:
			cnt=50;//stop
			break;
			
			case 16:
			cnt=999;//high speed
			break;
			
			case 8:
			keep_one_catch--;
			break;
			
			default:
			break;
		}
		
		switch(cnt)
		{
			case 1500:
			readline(rec);
//			xunji_motor();
//			check_xunji();
			HAL_GPIO_WritePin(STBY_GPIO_Port,STBY_Pin,GPIO_PIN_SET);
			xunji_set_action(data[7],data[8],data[5]);//should x_y_omg -> four pwm
			break;
			
			case 666:

			duoji_motor(key);
			duoji_execute();
			HAL_GPIO_WritePin(STBY_GPIO_Port,STBY_Pin,GPIO_PIN_SET);
			set_action(Data[7],Data[8],Data[5]);//should x_y_omg -> four pwm
			if(keep_one_catch==0)
			{
				catch_one();
				keep_one_catch--;
			}
			break;
			
			case 999:
			HAL_GPIO_WritePin(STBY_GPIO_Port,STBY_Pin,GPIO_PIN_SET);
			high_set_action(Data[7],Data[8],Data[5]);//should x_y_omg -> four pwm
			break;
			
			case 50:
			HAL_GPIO_WritePin(STBY_GPIO_Port,STBY_Pin,GPIO_PIN_RESET);
			break;
			
			default:
			break;
		}
		///////////////
		read_pin();
			dist=5;
			dist=rec[0]+rec[1]+rec[2]+rec[3]+rec[4];
			if(cnt==1500)
			{
				if(keep!=1)
				{
					if(dist==0)
					{
							data[5]=127;
							data[7]=127;
							data[8]=40;
							HAL_GPIO_WritePin(STBY_GPIO_Port,STBY_Pin,GPIO_PIN_SET);
							xunji_set_action(data[7],data[8],data[5]);//should x_y_omg -> four pwm
							motor_box();
							HAL_Delay(1000);
							
							cnt=50;
					}
				}
		   }
			//////////////////////////////////////
			if(cnt==1500)//brief method
			{
				read_pin();
				
				if(rec[2]==0)//mid
				{
					rec[2]=1;
					data[5]=128;
					data[7]=128;
					data[8]=22;
					HAL_GPIO_WritePin(STBY_GPIO_Port,STBY_Pin,GPIO_PIN_SET);
					xunji_set_action(data[7],data[8],data[5]);//should x_y_omg -> four pwm
				}
				if(rec[3]==0)//right_s
				{
					rec[3]=1;
					data[5]=140;
					data[7]=150;
					data[8]=22;
				}
				if(rec[1]==0)//left_s
				{
					rec[1]=1;
					data[5]=115;
					data[7]=100;
					data[8]=22;
				}
				if(rec[0]==0)//left
				{
					rec[0]=1;
					data[5]=55;
					data[7]=127;
					data[8]=127;
					//check_xunji();
				  HAL_GPIO_WritePin(STBY_GPIO_Port,STBY_Pin,GPIO_PIN_SET);
					xunji_set_action(data[7],data[8],data[5]);//should x_y_omg -> four pwm
				}
				if(rec[4]==0)//right
				{
					rec[4]=1;
					data[5]=195;
					data[7]=127;
					data[8]=127;
					//check_xunji();
				HAL_GPIO_WritePin(STBY_GPIO_Port,STBY_Pin,GPIO_PIN_SET);
				xunji_set_action(data[7],data[8],data[5]);//should x_y_omg -> four pwm
				}

			}
			
			///////////////
				motor_box();
  }
					/*
				read_pin();
				if( rec[1]==0 && rec[2]==1 && rec[3]==0)
				{
					data[5]=55;
					data[7]=127;
					data[8]=127;
					//check_xunji();
				  HAL_GPIO_WritePin(STBY_GPIO_Port,STBY_Pin,GPIO_PIN_SET);
					xunji_set_action(data[7],data[8],data[5]);//should x_y_omg -> four pwm
					motor_box();
					HAL_Delay(50);
					data[5]=127;
					data[7]=30;
					data[8]=127;
					HAL_GPIO_WritePin(STBY_GPIO_Port,STBY_Pin,GPIO_PIN_SET);
					xunji_set_action(data[7],data[8],data[5]);//should x_y_omg -> four pwm
					motor_box();
					HAL_Delay(50);
					HAL_GPIO_WritePin(STBY_GPIO_Port,STBY_Pin,GPIO_PIN_RESET);
				}*/
	/*
				while(keep == 1 && keep == 2)
				{
					__HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,pwm[1]);////BF
					__HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_2,pwm[2]);////BR
					__HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_3,pwm[0]);////AF
					__HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_4,pwm[3]);////AR
					read_pin();
					//
					if(keep==1)
					{
						 if(rec[0]==0)
						 {
							 keep=0;
						 }
					}
					//
					if(keep==2)
					{
						if(rec[4]==0)
						{
							keep=0;
						}
					}
					//
				}*/ 
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 0xFFFF;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 0xFFFF;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 0xFFFF;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 84-1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 999;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 0;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 0;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim5, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */

}

/**
  * @brief TIM8 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM8_Init(void)
{

  /* USER CODE BEGIN TIM8_Init 0 */

  /* USER CODE END TIM8_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM8_Init 1 */

  /* USER CODE END TIM8_Init 1 */
  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 164-1;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 20000-1;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim8, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM8_Init 2 */

  /* USER CODE END TIM8_Init 2 */
  HAL_TIM_MspPostInit(&htim8);

}

/**
  * @brief TIM9 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM9_Init(void)
{

  /* USER CODE BEGIN TIM9_Init 0 */

  /* USER CODE END TIM9_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM9_Init 1 */

  /* USER CODE END TIM9_Init 1 */
  htim9.Instance = TIM9;
  htim9.Init.Prescaler = 164-1;
  htim9.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim9.Init.Period = 20000-1;
  htim9.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim9.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim9) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim9, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim9, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM9_Init 2 */

  /* USER CODE END TIM9_Init 2 */
  HAL_TIM_MspPostInit(&htim9);

}

/**
  * @brief TIM13 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM13_Init(void)
{

  /* USER CODE BEGIN TIM13_Init 0 */

  /* USER CODE END TIM13_Init 0 */

  /* USER CODE BEGIN TIM13_Init 1 */

  /* USER CODE END TIM13_Init 1 */
  htim13.Instance = TIM13;
  htim13.Init.Prescaler = 840-1;
  htim13.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim13.Init.Period = 9999;
  htim13.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim13.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim13) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM13_Init 2 */

  /* USER CODE END TIM13_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, DO_Pin|AF1_Pin|AF2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, CS_Pin|BF1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, CLK_Pin|STBY_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(BF2_GPIO_Port, BF2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, AR1_Pin|AR2_Pin|BR2_Pin|BR1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : R4_Pin R3_Pin R2_Pin R1_Pin 
                           R0_Pin */
  GPIO_InitStruct.Pin = R4_Pin|R3_Pin|R2_Pin|R1_Pin 
                          |R0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : DI_Pin */
  GPIO_InitStruct.Pin = DI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DI_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : DO_Pin AF1_Pin AF2_Pin */
  GPIO_InitStruct.Pin = DO_Pin|AF1_Pin|AF2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : CS_Pin BF1_Pin */
  GPIO_InitStruct.Pin = CS_Pin|BF1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : CLK_Pin STBY_Pin */
  GPIO_InitStruct.Pin = CLK_Pin|STBY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : R6_Pin R7_Pin R8_Pin R9_Pin */
  GPIO_InitStruct.Pin = R6_Pin|R7_Pin|R8_Pin|R9_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : R5_Pin */
  GPIO_InitStruct.Pin = R5_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(R5_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BF2_Pin */
  GPIO_InitStruct.Pin = BF2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BF2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : AR1_Pin AR2_Pin BR2_Pin BR1_Pin */
  GPIO_InitStruct.Pin = AR1_Pin|AR2_Pin|BR2_Pin|BR1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
