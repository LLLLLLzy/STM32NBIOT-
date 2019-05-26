#include "led.h"


//初始化PB5和PE5为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOB, ENABLE);	 //使能PA,PD端口时钟

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				//LED0-->PB5 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                       //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                      //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					//根据设定参数初始化GPIOB5

 GPIO_ResetBits(GPIOB,GPIO_Pin_5);                                        //PB5 输出高

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				//LED0-->PB6 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                       //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                      //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					//根据设定参数初始化GPIOB6

 GPIO_ResetBits(GPIOB,GPIO_Pin_6);                                        //PB6 输出高
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				//LED0-->PB7 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                       //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                      //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					//根据设定参数初始化GPIOB5

 GPIO_ResetBits(GPIOB,GPIO_Pin_7);                                        //PB7 输出高


}

