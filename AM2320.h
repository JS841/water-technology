#ifndef __AM2320_H
#define	__AM2320_H


#include "stm32f10x.h"

/************************** AM2320 数据类型定义********************************/
typedef struct
{
	uint8_t  humi_int;		//湿度的整数部分
	uint8_t  humi_deci;	 	//湿度的小数部分
	uint8_t  temp_int;	 	//温度的整数部分
	uint8_t  temp_deci;	 	//温度的小数部分
	uint8_t  check_sum;	 	//校验和
		                 
} AM2320_Data_TypeDef;

#define AM2320_Dout_GPIO_PIN  GPIO_Pin_9///////////////////////未作修改，直接用20230515
#define AM2320_SCL_GPIO_PIN  	GPIO_Pin_8
#define AM2320_Dout_GPIO_PORT   GPIOB

/************************** AM2320 函数宏定义********************************/
#define      AM2320_Dout_0	                            GPIO_ResetBits ( AM2320_Dout_GPIO_PORT, AM2320_Dout_GPIO_PIN ) 
#define      AM2320_Dout_1	                            GPIO_SetBits ( AM2320_Dout_GPIO_PORT, AM2320_Dout_GPIO_PIN ) 

#define      AM2320_Dout_IN()	                          GPIO_ReadInputDataBit ( AM2320_Dout_GPIO_PORT, GPIO_Pin_9 ) 

static void AM2320_GPIO_Config ( void );
void AM2320_Init ( void );
static void AM2320_Mode_IPU(void);
static void AM2320_Mode_Out_PP(void);
static uint8_t AM2320_ReadByte ( void );
uint8_t AM2320_Read_TempAndHumidity(AM2320_Data_TypeDef *AM2320_Data);


#endif 

