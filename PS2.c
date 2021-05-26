#include "PS2.h"
#include "dwt_stm32_delay.h"
	uint16_t Handkey;
uint8_t Comd[2]={0x01,0x42};	//开始命令。请求数据
uint8_t Data[9]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //数据存储数组
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
	};	//按键值与按键明
//set_the_every_buttom_



void PS2_Cmd(uint8_t CMD)
{
	volatile uint16_t ref=0x01;//check_bits
	Data[1] = 0;
	for(ref=0x01;ref<0x0100;ref<<=1)//check the bit and set the restart bit (data[1])//
	{
		if(ref&CMD)//1&0=0
		{
			DO_H;                   //输出以为控制位
		}
		else DO_L;

		CLK_H;                        //时钟拉高//then reset_CLK transend data
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
	PS2_Cmd(Comd[0]);  //开始命令
	PS2_Cmd(Comd[1]);  //请求数据
	CS_H;
	if( Data[1] == 0X73)   return 0 ;
	else return 1;

}

void PS2_ReadData(void)
{
	volatile uint8_t byte=0;//check the every send
	volatile uint16_t ref=0x01;//comparative bit

	CS_L;

	PS2_Cmd(Comd[0]);  //开始命令
	PS2_Cmd(Comd[1]);  //请求数据

	for(byte=2;byte<9;byte++)          //开始接受数据//check all bit if set then get its data
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

	Handkey=(Data[4]<<8)|Data[3];     //这是16个按键  按下为0， 未按下为1
	for(index=0;index<16;index++)
	{	    
		if((Handkey&(1<<(MASK[index]-1)))==0)//cycle to check every buttom
		return index+1;
	}
	return 0;          //没有任何按键按下
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

