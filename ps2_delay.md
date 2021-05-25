```c
#include "PS2.h"
#include "dwt_stm32_delay.h"
	uint16_t Handkey;
uint8_t Comd[2]={0x01,0x42};	//¿ªÊ¼ÃüÁî¡£ÇëÇóÊý¾Ý
uint8_t Data[9]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //Êý¾Ý´æ´¢Êý×é
uint16_t MASK[]={
    PSB_SELECT,
    PSB_L3,
    PSB_R3 ,
    PSB_START,
    PSB_PAD_UP,
    PSB_PAD_RIGHT,
    PSB_PAD_DOWN,
    PSB_PAD_LEFT,
    PSB_L2,
    PSB_R2,
    PSB_L1,
    PSB_R1 ,
    PSB_GREEN,
    PSB_RED,
    PSB_BLUE,
    PSB_PINK
	};	//°´¼üÖµÓë°´¼üÃ÷
//set_the_every_buttom_



void PS2_Cmd(uint8_t CMD)
{
	volatile uint16_t ref=0x01;//check_bits
	Data[1] = 0;
	for(ref=0x01;ref<0x0100;ref<<=1)//check the bit and set the restart bit (data[1])//
	{
		if(ref&CMD)//1&0=0
		{
			DO_H;                   //Êä³öÒÔÎª¿ØÖÆÎ»
		}
		else DO_L;

		CLK_H;                        //Ê±ÖÓÀ­¸ß//then reset_CLK transend data
		DWT_Delay_us(10);
		CLK_L;
		DWT_Delay_us(10);
		CLK_H;
		if(DI)
			Data[1] = ref|Data[1];//1 | 0 =1
	}
	 DWT_Delay_us(16);
}

uint8_t PS2_RedLight(void)//a position for waiting data
{
	CS_L;
	PS2_Cmd(Comd[0]);  //¿ªÊ¼ÃüÁî
	PS2_Cmd(Comd[1]);  //ÇëÇóÊý¾Ý
	CS_H;
	if( Data[1] == 0X73)   return 0 ;
	else return 1;

}

void PS2_ReadData(void)
{
	volatile uint8_t byte=0;//check the every send
	volatile uint16_t ref=0x01;//comparative bit

	CS_L;

	PS2_Cmd(Comd[0]);  //¿ªÊ¼ÃüÁî
	PS2_Cmd(Comd[1]);  //ÇëÇóÊý¾Ý

	for(byte=2;byte<9;byte++)          //¿ªÊ¼½ÓÊÜÊý¾Ý//check all bit if set then get its data
	{
		for(ref=0x01;ref<0x100;ref<<=1)
		{
			CLK_H;
			DWT_Delay_us(10);
			CLK_L;//reset CLK for transend data
			DWT_Delay_us(10);
			CLK_H;
		      if(DI)//
		      Data[byte] = ref|Data[byte];//get the data
		}
        DWT_Delay_us(16);
	}
	CS_H;	
}

uint8_t PS2_DataKey()//check the buttom
{
	uint8_t index;

	PS2_ClearData();
	PS2_ReadData();

	Handkey=(Data[4]<<8)|Data[3];     //ÕâÊÇ16¸ö°´¼ü  °´ÏÂÎª0£¬ Î´°´ÏÂÎª1
	for(index=0;index<16;index++)
	{	    
		if((Handkey&(1<<(MASK[index]-1)))==0)//cycle to check every buttom
		return index+1;
	}
	return 0;          //Ã»ÓÐÈÎºÎ°´¼ü°´ÏÂ
}

uint8_t PS2_AnologData(uint8_t button)//set different anologdata
{
	return Data[button];
}

void PS2_ClearData()
{
	uint8_t a;
	for(a=0;a<9;a++)
		Data[a]=0x00;//clear data and init to 0x00
}
void PS2_ShortPoll(void)
{
	CS_L;
	DWT_Delay_us(16);;
	PS2_Cmd(0x01);
	PS2_Cmd(0x42);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	CS_H;
	DWT_Delay_us(16);
}
void PS2_EnterConfing(void)
{
	CS_L;
	DWT_Delay_us(16);
	PS2_Cmd(0x01);
	PS2_Cmd(0x42);
	PS2_Cmd(0x00);
	PS2_Cmd(0x01);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	CS_H;
	DWT_Delay_us(16);
}
void PS2_TurnOnAnalogMode(void)
{
	CS_L;
	PS2_Cmd(0x01);
	PS2_Cmd(0x42);
	PS2_Cmd(0x00);
	PS2_Cmd(0x01);
	PS2_Cmd(0xEE);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	CS_H;
	DWT_Delay_us(16);
}
void PS2_VibrationMode(void)
{
	CS_L;
	DWT_Delay_us(16);
	PS2_Cmd(0x01);
	PS2_Cmd(0x4D);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x01);
	CS_H;
	DWT_Delay_us(16);
}
void PS2_ExitConfing(void)
{
	CS_L;
	DWT_Delay_us(16);
	PS2_Cmd(0x01);
	PS2_Cmd(0x43);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x5A);
	PS2_Cmd(0x5A);
	PS2_Cmd(0x5A);
	PS2_Cmd(0x5A);
	PS2_Cmd(0x5A);
	CS_H;
	DWT_Delay_us(16);
}
void PS2_SetInit(void)
{
	PS2_ShortPoll();
	PS2_ShortPoll();
	PS2_ShortPoll();
	PS2_EnterConfing();
	PS2_TurnOnAnalogMode();
	PS2_VibrationMode();
	PS2_ExitConfing();
}
void PS2_Vibration(uint8_t motor1,uint8_t motor2)
{
	CS_L;
	DWT_Delay_us(16);
	PS2_Cmd(0x01);
	PS2_Cmd(0x42);
	PS2_Cmd(0x00);
	PS2_Cmd(motor1);
	PS2_Cmd(motor2);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	CS_H;
	DWT_Delay_us(16);
}

```

```c
#include "dwt_stm32_delay.h"

uint32_t DWT_Delay_Init(void) {
  /* Disable TRC */
  CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk; // ~0x01000000;
  /* Enable TRC */
  CoreDebug->DEMCR |=  CoreDebug_DEMCR_TRCENA_Msk; // 0x01000000;
 
  /* Disable clock cycle counter */
  DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; //~0x00000001;
  /* Enable  clock cycle counter */
  DWT->CTRL |=  DWT_CTRL_CYCCNTENA_Msk; //0x00000001;
 
  /* Reset the clock cycle counter value */
  DWT->CYCCNT = 0;
 
     /* 3 NO OPERATION instructions */
     __ASM volatile ("NOP");
     __ASM volatile ("NOP");
  __ASM volatile ("NOP");
 
  /* Check if clock cycle counter has started */
     if(DWT->CYCCNT)
     {
       return 0; /*clock cycle counter started*/
     }
     else
  {
    return 1; /*clock cycle counter not started*/
  }
}


```

```c
#ifndef __PS2_H
#define __PS2_H
#include "stdint.h"
#include "stm32f4xx_hal.h"
#include "dwt_stm32_delay.h"



#define DI_Pin GPIO_PIN_4
#define DI_GPIO_Port GPIOA
#define DO_Pin GPIO_PIN_4
#define DO_GPIO_Port GPIOC
#define CS_Pin GPIO_PIN_0
#define CS_GPIO_Port GPIOB
#define CLK_Pin GPIO_PIN_7
#define CLK_GPIO_Port GPIOE

#define DO_H HAL_GPIO_WritePin(DO_GPIO_Port,DO_Pin,GPIO_PIN_SET)
#define DO_L HAL_GPIO_WritePin(DO_GPIO_Port,DO_Pin,GPIO_PIN_RESET)

#define DI HAL_GPIO_ReadPin(DI_GPIO_Port,DI_Pin)

#define CS_H HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin,GPIO_PIN_SET)
#define CS_L HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin,GPIO_PIN_RESET)
#define CLK_H HAL_GPIO_WritePin(CLK_GPIO_Port,CLK_Pin,GPIO_PIN_SET)
#define CLK_L HAL_GPIO_WritePin(CLK_GPIO_Port,CLK_Pin,GPIO_PIN_RESET)


#define PSB_SELECT      1
#define PSB_L3          2
#define PSB_R3          3
#define PSB_START       4
#define PSB_PAD_UP      5
#define PSB_PAD_RIGHT   6
#define PSB_PAD_DOWN    7
#define PSB_PAD_LEFT    8
#define PSB_L2         9
#define PSB_R2          10
#define PSB_L1          11
#define PSB_R1          12
#define PSB_GREEN       13
#define PSB_RED         14
#define PSB_BLUE        15
#define PSB_PINK        16
#define PSB_TRIANGLE    13
#define PSB_CIRCLE      14
#define PSB_CROSS       15
#define PSB_SQUARE      26
#define PSS_RX 5                //ÓÒÒ¡¸ËXÖáÊý¾Ý
#define PSS_RY 6
#define PSS_LX 7
#define PSS_LY 8

#define DO_H HAL_GPIO_WritePin(DO_GPIO_Port,DO_Pin,GPIO_PIN_SET)
#define DO_L HAL_GPIO_WritePin(DO_GPIO_Port,DO_Pin,GPIO_PIN_RESET)

#define DI HAL_GPIO_ReadPin(DI_GPIO_Port,DI_Pin)

#define CS_H HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin,GPIO_PIN_SET)
#define CS_L HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin,GPIO_PIN_RESET)
#define CLK_H HAL_GPIO_WritePin(CLK_GPIO_Port,CLK_Pin,GPIO_PIN_SET)
#define CLK_L HAL_GPIO_WritePin(CLK_GPIO_Port,CLK_Pin,GPIO_PIN_RESET)
#define delay_16us  DWT_Delay_us(16)
void PS2_SetInit(void);
uint8_t PS2_RedLight(void);//ÅÐ¶ÏÊÇ·ñÎªºìµÆÄ£Ê½
void PS2_ReadData(void);
void PS2_Cmd(uint8_t CMD);		  //
uint8_t PS2_DataKey(void);		  //¼üÖµ¶ÁÈ¡
uint8_t PS2_AnologData(uint8_t button); //µÃµ½Ò»¸öÒ¡¸ËµÄÄ£ÄâÁ¿
void PS2_ClearData(void);	  //Çå³ýÊý¾Ý»º³åÇø
//void delay_us(uint16_t us);



```

```c
#ifndef DWT_STM32_DELAY_H
#define DWT_STM32_DELAY_H

#ifdef __cplusplus
extern "C" {
#endif
 
#include "stm32f4xx_hal.h"
 
/**
 * @brief  Initializes DWT_Cycle_Count for DWT_Delay_us function
 * @return Error DWT counter
 *         1: DWT counter Error
 *         0: DWT counter works
 */
uint32_t DWT_Delay_Init(void);
 
/**
 * @brief  This function provides a delay (in microseconds)
 * @param  microseconds: delay in microseconds
 */
__STATIC_INLINE void DWT_Delay_us(volatile uint32_t microseconds)
{
  uint32_t clk_cycle_start = DWT->CYCCNT;
 
  /* Go to number of cycles for system */
  microseconds *= (HAL_RCC_GetHCLKFreq() / 1000000);
 
  /* Delay till end */
  while ((DWT->CYCCNT - clk_cycle_start) < microseconds);
}
 
#ifdef __cplusplus
}
#endif
 
#endif

```

申明：delay摘引自大佬

<img src="C:\Users\86156\Desktop\2021RM校内赛\rm校内赛。电控\1621923651(1).png" alt="1621923651(1)" style="zoom:50%;" />

ps2的驱动：有相关例程

以下几点想要强调一下：

1. **时基非常重要**，正确了才能使用好ps2，本质上就是**spi总线**传数据
2. 关于数据传输的小白思想，基本上都是int类型传输一个1或者0，实际上这样非常浪费，通过阅读例程，可以很明显的发现，一个int类型其实是可以直接传输八个01，也就是一个字节，这样非常高效，只是对后来的阅读者不好，容易变成屎堆。。。。思想值得借鉴，做好注释！