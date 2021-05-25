# 电控主程序

为防过度冗杂

{ motor，pid，ps2，steer，xunji，configuration }分区块，感兴趣可以去看看！

###### 核心：遥控：封装{ 循迹和舵机 }

```c
	uint8_t Mecanum[4]={1,1,1,1};//控制电机正反转的标志位
	int speed[4];
	double	check[3]={0,0,0};///encoder 计算xyz轴的速度test//0_Vx//1_Vy//2_omg
	int pwm[4]={0,0,0,0};//set_pwm
			uint8_t key=0;

	//xunji
	uint8_t keep=1;
	uint8_t keep_one_catch = 1;//一键夹取的标志位，控制函数只执行一次
extern int rec[10];
extern int xunji_read[2];
	//
	extern int duoji[3];
	int catch_flag=1;
	//usart1_tof
	uint8_t hello[] = " hello,let'go now ";
	uint8_t receive [13]={0};//串口接收数据储存
	int dist;//for catch，当读到全为零的时候，执行出库函数
	//
extern uint16_t Handkey;
extern uint8_t Comd[2];	//
extern uint8_t Data[9]; //
extern uint16_t MASK[16];	//
	extern int data[16];//xunji_data
```

```c
void get_x_y_raw(uint16_t Vaf,uint16_t Vbf,uint16_t Vbr,uint16_t Var)//计算得到每个轮子当前的速度，运动学结算
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
 void motor_box(void)//驱动盒子，执行当前pwm输出并正反转信号
 {
	 set_bit();//need pwm[x]//->//Mecanum[x]//
			motor(Mecanum);//-> F_&_B //
			no_fast();//restrict_pwm//
			__HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,pwm[1]);////BF
			__HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_2,pwm[2]);////BR
			__HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_3,pwm[0]);////AF
			__HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_4,pwm[3]);////AR
 }
 void duoji_execute(void)//驱动舵机
 {
	 duoji_constrain();
	 __HAL_TIM_SetCompare(&htim8,TIM_CHANNEL_4,duoji[0]);//up_down
	 __HAL_TIM_SetCompare(&htim9,TIM_CHANNEL_1,duoji[1]);//forth_back
	 __HAL_TIM_SetCompare(&htim9,TIM_CHANNEL_2,duoji[2]);//catch
 }
 void catch_first(void)//单次抓取获得方块
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
```

```c
void catch_second(void)//第二步夹取方块
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
	 
```

由于stm32![1621924808(1)](C:\Users\86156\Desktop\2021RM校内赛\rm校内赛。电控\1621924808(1).png)的一些函数功能存在，所以单独放到main程序里面来说

不要忘记初始化函数！

```c
	key=PS2_DataKey();
```

循环头部一直读取遥控器按键位置

```c
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
```

两个switch表示的是读取位置给小车设置模式

{ 循迹模式，高速档，低速档，停止，舵机的遥控，舵机一键夹取，遥控模式切换 }

最上层：遥控：直接跟着while（1）后面，每次都进行

次级：循迹模式（一键夹取模式），摇杆模式（输出xyz轴的大小），高低速档位，舵机操作）

所以不难看出，只要按停，优先级最高，直接停下







这是第二种循迹算法：没什么好说的，最外面的读到白线停止xy轴，只执行z轴自转

```c
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
```



```c
motor_box();
```

最后执行驱动盒子





最后在讲一下我是怎么封装的

：根据xyz轴数据，运动学结算，得到每个轮子应该得到的pwm大小

带符号，扔到set_action();里面设置正反转标志位，pwm取绝对值；

标志位影响tb6612的引脚信号，确定正反转，pwm绝对值确定轮子转速

motor——box驱动小车！就ok啦