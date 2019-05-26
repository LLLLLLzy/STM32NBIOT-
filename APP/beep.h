#ifndef __BEEP_H
#define __BEEP_H

#include "stm32f10x.h"

//#define BEEP_ON GPIO_SetBits(GPIOA,GPIO_Pin_11)  //·äÃùÆ÷
//#define BEEP_OFF GPIO_ResetBits(GPIOA,GPIO_Pin_11)


#define BEEP_REV GPIO_WriteBit(GPIOA, GPIO_Pin_11,(BitAction)(1-(GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_11))))


void BEEP_Init(void);

#endif
