#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "stm32f10x_conf.h"

#include "usart1.h"
#include "delay.h"


#include "AM2320.h"




/*
		AM2320	SDA											PB9
						SCL											PB8
*/

//管脚初始化
static void AM2320_GPIO_Config ( void )
{		

	GPIO_InitTypeDef GPIO_InitStructure; 

  RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOB, ENABLE );	
 
														   
  GPIO_InitStructure.GPIO_Pin = AM2320_Dout_GPIO_PIN|AM2320_SCL_GPIO_PIN;	
 

  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
 
 
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
 

  	GPIO_Init (GPIOB, &GPIO_InitStructure );		  
}

void AM2320_Init ( void )
{
	AM2320_GPIO_Config ();
	GPIO_ResetBits(GPIOB, GPIO_Pin_8);
	GPIO_SetBits(GPIOB, GPIO_Pin_9);               // 拉高引脚
}
//AM2320变成上拉输入模式
static void AM2320_Mode_IPU(void)

{	  
		GPIO_InitTypeDef GPIO_InitStructure;

		RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOB, ENABLE );	
	
		GPIO_InitStructure.GPIO_Pin = AM2320_Dout_GPIO_PIN;
				
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ; //浮空输入
			
		GPIO_Init(AM2320_Dout_GPIO_PORT, &GPIO_InitStructure);	 
}

//AM2320的DATA变为推挽输出模式
static void AM2320_Mode_Out_PP(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;

		RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOB, ENABLE );	
	
  	GPIO_InitStructure.GPIO_Pin = AM2320_Dout_GPIO_PIN;	
 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 
  	GPIO_Init(AM2320_Dout_GPIO_PORT, &GPIO_InitStructure);	 	 
}
//读取AM2320的字节 MSB先行
static uint8_t AM2320_ReadByte ( void )
{
	uint8_t i, temp=0;
 
	for(i=0;i<8;i++)    
	{	 
		//每bit以50us低电平标志开始	
		while(AM2320_Dout_IN()==Bit_RESET);
		//AM2320以26-28us的高电平表示0，以70us高电平表示1
    delay_us(40); 
		
		if(AM2320_Dout_IN()==Bit_SET)	//40us延时后高电平表示数据 		1
		{
			//等待数据1  的高电平结束
			while(AM2320_Dout_IN()==Bit_SET);
 
			temp|=(uint8_t)(0x01<<(7-i));  //把7-i位置1  MSB先行
		}
		else	 // 40us后是低电平
			{			   
			temp&=(uint8_t)~(0x01<<(7-i)); //把7-i位置0  MSB先行
		}
	}
	//printf("\n\rTEMP_PM=%d",temp);
	return temp;
	
}

 /*
 一次完整的数据是40bit
8bit 湿度整数   8bit 湿度小数 +8bit 温度整数  +8bit温度小数		+8bit 校验和
 */

uint8_t AM2320_Read_TempAndHumidity(AM2320_Data_TypeDef *AM2320_Data)
{  
	u8 calculation;
	u8 TEMP_AM;
	//printf("\r\n AM2320\r\n");  
	//输出模式
	AM2320_Mode_Out_PP();
	//主机拉低
	AM2320_Dout_0;
	//延时
 delay_ms(18);
//主机拉高
	AM2320_Dout_1; 
 delay_us(30);   //延时30us
	
	//主机设为输入判断响应信号
	AM2320_Mode_IPU();
	

	
	TEMP_AM=AM2320_Dout_IN();
	
	//判断丛集是否有低电平响应信号 
	if(TEMP_AM==Bit_RESET)     
	{
		//轮训知道从机发出80us高电平 标志信号结束
		while(AM2320_Dout_IN()==Bit_RESET);
		//轮训知道从机发出80us高电平 标志信号结束
		while(AM2320_Dout_IN()==Bit_SET);
		//开始接收数据
		AM2320_Data->humi_int= AM2320_ReadByte();
 
		AM2320_Data->humi_deci= AM2320_ReadByte();
 
		AM2320_Data->temp_int= AM2320_ReadByte();
 
		AM2320_Data->temp_deci= AM2320_ReadByte();
 
		AM2320_Data->check_sum= AM2320_ReadByte();
		
	//	printf("/r/nh_h:%x,h_l:%x,t_h:%x,t_l:%x,c:%x",AM2320_Data->humi_int,AM2320_Data->humi_deci,AM2320_Data->temp_int,AM2320_Data->temp_deci,AM2320_Data->check_sum);
		//打印接收数据
		AM2320_Mode_Out_PP();
		//主机拉高
		AM2320_Dout_1;
		calculation = AM2320_Data->humi_int + AM2320_Data->humi_deci + AM2320_Data->temp_int+ AM2320_Data->temp_deci;
		//printf("\r\ncheck_sum:%x;calculation:%x",AM2320_Data->check_sum,calculation);
		//检查数据是否之前
		if(AM2320_Data->check_sum == calculation)
			{
			return SUCCESS;
			}
		else 
			{
			//printf("\r\nChecksum Error!check_sum:%x;calculation:%x",AM2320_Data->check_sum,\
			AM2320_Data->humi_int + AM2320_Data->humi_deci + AM2320_Data->temp_int+ AM2320_Data->temp_deci);
				return ERROR;
		}
	}
	else
		{
			//printf("\r\nGet Device Error!");
			return ERROR;
	}
}












































