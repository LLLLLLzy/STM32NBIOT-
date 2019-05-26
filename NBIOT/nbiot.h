#ifndef __BC95B5_H__
#define __BC95B5_H__	 
#include "sys.h"


#define swap16(x) (x&0XFF)<<8|(x&0XFF00)>>8	//�ߵ��ֽڽ����궨��

extern u8 Scan_Wtime;

void sim_at_response(u8 mode);	
u8* NB_check_cmd(u8 *str);
u8 NB_send_cmd(u8 *cmd,u8 *Re1,u8 *Re2,u8 *Re3,u16 waittime);
u8 NB_wait_request(u8 *request ,u16 waittime);
u8 NB_chr2hex(u8 chr);
u8 NB_hex2chr(u8 hex);
void BC95_RECData(void);

#endif

typedef struct
{
   uint8_t CSQ;    
	 uint8_t Socketnum;   //���
	 uint8_t reclen;   //��ȡ�����ݵĳ���
   uint8_t res;      
   uint8_t recdatalen[10];
   uint8_t recdata[100];
	 uint8_t uart1len[10];
	 uint8_t senddata[100];
} BC95;


