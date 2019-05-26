#ifndef __ADC_H
#define	__ADC_H

#include "stm32f10x.h"

// 注意：用作ADC采集的IO必须没有复用，否则采集电压会有影响
/********************ADC1输入通道（引脚）配置**************************/
#define    macADC_APBxClock_FUN             RCC_APB2PeriphClockCmd
#define    macADC_CLK                       RCC_APB2Periph_ADC1

#define    macADC_GPIO_APBxClock_FUN        RCC_APB2PeriphClockCmd
#define    macADC_GPIO_CLK                  RCC_APB2Periph_GPIOC  
#define    macADC_PORT                      GPIOC

// 转换通道个数
#define    macNOFCHANEL											1

#define    macADC_PIN1                      GPIO_Pin_1
#define    macADC_CHANNEL1                  ADC_Channel_10



// ADC1 对应 DMA1通道1，ADC3对应DMA2通道5，ADC2没有DMA功能
#define    macADCx                          ADC1
#define    macADC_DMA_CHANNEL               DMA1_Channel1


/**************************函数声明********************************/
void               ADCx_Init                               (void);


#endif /* __ADC_H */

