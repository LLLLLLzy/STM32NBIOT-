#include "led.h"


//��ʼ��PB5��PE5Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PA,PD�˿�ʱ��

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				//LED0-->PB5 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                       //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                      //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					//�����趨������ʼ��GPIOB5

 GPIO_ResetBits(GPIOB,GPIO_Pin_5);                                        //PB5 �����

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				//LED0-->PB6 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                       //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                      //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					//�����趨������ʼ��GPIOB6

 GPIO_ResetBits(GPIOB,GPIO_Pin_6);                                        //PB6 �����
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				//LED0-->PB7 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                       //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                      //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					//�����趨������ʼ��GPIOB5

 GPIO_ResetBits(GPIOB,GPIO_Pin_7);                                        //PB7 �����


}

