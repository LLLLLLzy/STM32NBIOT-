/*-------------------------------------------------------------------------------
�ļ����ƣ�beep.c
�ļ�����������Ӳ���������÷������˿ڣ��򿪶�Ӧ�ļĴ���        
��    ע����
---------------------------------------------------------------------------------*/
#include "beep.h"
/*-------------------------------------------------------------------------------
�������ƣ�BEEP_Init
������������ʼ��BEEP��ض˿ڣ��򿪶˿�ʱ�ӣ����ö˿����  
�����������
���ز�������
��    ע����
---------------------------------------------------------------------------------*/
void BEEP_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  //��PA��ʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	//PA11��������
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	//�˿��ٶ�
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//�˿�ģʽ����Ϊ�������ģʽ
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	//��ʼ����Ӧ�Ķ˿�
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PA,PC�˿�ʱ��
	/*LCD5110_LCD_CTRL_PORT*/
 	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_12;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);

}
