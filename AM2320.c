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

//�ܽų�ʼ��
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
	GPIO_SetBits(GPIOB, GPIO_Pin_9);               // ��������
}
//AM2320�����������ģʽ
static void AM2320_Mode_IPU(void)

{	  
		GPIO_InitTypeDef GPIO_InitStructure;

		RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOB, ENABLE );	
	
		GPIO_InitStructure.GPIO_Pin = AM2320_Dout_GPIO_PIN;
				
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ; //��������
			
		GPIO_Init(AM2320_Dout_GPIO_PORT, &GPIO_InitStructure);	 
}

//AM2320��DATA��Ϊ�������ģʽ
static void AM2320_Mode_Out_PP(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;

		RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOB, ENABLE );	
	
  	GPIO_InitStructure.GPIO_Pin = AM2320_Dout_GPIO_PIN;	
 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 
  	GPIO_Init(AM2320_Dout_GPIO_PORT, &GPIO_InitStructure);	 	 
}
//��ȡAM2320���ֽ� MSB����
static uint8_t AM2320_ReadByte ( void )
{
	uint8_t i, temp=0;
 
	for(i=0;i<8;i++)    
	{	 
		//ÿbit��50us�͵�ƽ��־��ʼ	
		while(AM2320_Dout_IN()==Bit_RESET);
		//AM2320��26-28us�ĸߵ�ƽ��ʾ0����70us�ߵ�ƽ��ʾ1
    delay_us(40); 
		
		if(AM2320_Dout_IN()==Bit_SET)	//40us��ʱ��ߵ�ƽ��ʾ���� 		1
		{
			//�ȴ�����1  �ĸߵ�ƽ����
			while(AM2320_Dout_IN()==Bit_SET);
 
			temp|=(uint8_t)(0x01<<(7-i));  //��7-iλ��1  MSB����
		}
		else	 // 40us���ǵ͵�ƽ
			{			   
			temp&=(uint8_t)~(0x01<<(7-i)); //��7-iλ��0  MSB����
		}
	}
	//printf("\n\rTEMP_PM=%d",temp);
	return temp;
	
}

 /*
 һ��������������40bit
8bit ʪ������   8bit ʪ��С�� +8bit �¶�����  +8bit�¶�С��		+8bit У���
 */

uint8_t AM2320_Read_TempAndHumidity(AM2320_Data_TypeDef *AM2320_Data)
{  
	u8 calculation;
	u8 TEMP_AM;
	//printf("\r\n AM2320\r\n");  
	//���ģʽ
	AM2320_Mode_Out_PP();
	//��������
	AM2320_Dout_0;
	//��ʱ
 delay_ms(18);
//��������
	AM2320_Dout_1; 
 delay_us(30);   //��ʱ30us
	
	//������Ϊ�����ж���Ӧ�ź�
	AM2320_Mode_IPU();
	

	
	TEMP_AM=AM2320_Dout_IN();
	
	//�жϴԼ��Ƿ��е͵�ƽ��Ӧ�ź� 
	if(TEMP_AM==Bit_RESET)     
	{
		//��ѵ֪���ӻ�����80us�ߵ�ƽ ��־�źŽ���
		while(AM2320_Dout_IN()==Bit_RESET);
		//��ѵ֪���ӻ�����80us�ߵ�ƽ ��־�źŽ���
		while(AM2320_Dout_IN()==Bit_SET);
		//��ʼ��������
		AM2320_Data->humi_int= AM2320_ReadByte();
 
		AM2320_Data->humi_deci= AM2320_ReadByte();
 
		AM2320_Data->temp_int= AM2320_ReadByte();
 
		AM2320_Data->temp_deci= AM2320_ReadByte();
 
		AM2320_Data->check_sum= AM2320_ReadByte();
		
	//	printf("/r/nh_h:%x,h_l:%x,t_h:%x,t_l:%x,c:%x",AM2320_Data->humi_int,AM2320_Data->humi_deci,AM2320_Data->temp_int,AM2320_Data->temp_deci,AM2320_Data->check_sum);
		//��ӡ��������
		AM2320_Mode_Out_PP();
		//��������
		AM2320_Dout_1;
		calculation = AM2320_Data->humi_int + AM2320_Data->humi_deci + AM2320_Data->temp_int+ AM2320_Data->temp_deci;
		//printf("\r\ncheck_sum:%x;calculation:%x",AM2320_Data->check_sum,calculation);
		//��������Ƿ�֮ǰ
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












































