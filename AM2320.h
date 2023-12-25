#ifndef __AM2320_H
#define	__AM2320_H


#include "stm32f10x.h"

/************************** AM2320 �������Ͷ���********************************/
typedef struct
{
	uint8_t  humi_int;		//ʪ�ȵ���������
	uint8_t  humi_deci;	 	//ʪ�ȵ�С������
	uint8_t  temp_int;	 	//�¶ȵ���������
	uint8_t  temp_deci;	 	//�¶ȵ�С������
	uint8_t  check_sum;	 	//У���
		                 
} AM2320_Data_TypeDef;

#define AM2320_Dout_GPIO_PIN  GPIO_Pin_9///////////////////////δ���޸ģ�ֱ����20230515
#define AM2320_SCL_GPIO_PIN  	GPIO_Pin_8
#define AM2320_Dout_GPIO_PORT   GPIOB

/************************** AM2320 �����궨��********************************/
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

