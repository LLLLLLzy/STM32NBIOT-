#include "nbiot.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h"   	 
#include "delay.h"
#include "key.h"	 	 	 	 	 
#include "string.h"    
#include "usart2.h" 

u8 Scan_Wtime = 0;//����ɨ����Ҫ��ʱ��
u8 BT_Scan_mode=0;//ɨ���豸ģʽ��־
char *extstrx;
extern char  RxBuffer1[100],RxCounter;
BC95 BC95_Status;
__IO u8 jie[255];   //��Ž�����Ϣ

//usmart֧�ֲ��� 
//���յ���ATָ��Ӧ�����ݷ��ظ����Դ���
//mode:0,������USART2_RX_STA;
//     1,����USART2_RX_STA;
void sim_at_response(u8 mode)
{
    if(USART2_RX_STA&0X8000)		//���յ�һ��������
    {
        USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;//��ӽ�����
        printf("%s",USART2_RX_BUF);	//���͵�����
        if(mode)USART2_RX_STA=0;
    }
}
//////////////////////////////////////////////////////////////////////////////////
//ATK-NB �������(���Ų��ԡ����Ų��ԡ�GPRS���ԡ���������)���ô���

//NB���������,�����յ���Ӧ��
//str:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����
//����,�ڴ�Ӧ������λ��(str��λ��)
u8* NB_check_cmd(u8 *str)
{
    char *strx=0;
    if(USART2_RX_STA&0X8000)		//���յ�һ��������
    {
        USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;//��ӽ�����
        strx=strstr((const char*)USART2_RX_BUF,(const char*)str);
    }
    return (u8*)strx;
}
//��NB��������
//cmd:���͵������ַ���(����Ҫ��ӻس���),��cmd<0XFF��ʱ��,��������(���緢��0X1A),���ڵ�ʱ�����ַ���.
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8 NB_send_cmd(u8 *cmd,u8 *Re1,u8 *Re2,u8 *Re3,u16 waittime)
{

    u8 res=0;
    USART2_RX_STA=0;
    if((u32)cmd<=0XFF)
    {
        while(DMA1_Channel7->CNDTR!=0);	//�ȴ�ͨ��7�������
        USART2->DR=(u32)cmd;
    }else
    {
        u2_printf("%s\r\n",cmd);    //��������
        printf("ATSEND--> %s\r\n",cmd);       //��ӡ����
    }

    if(waittime==500)              //11s����ش�������(ɨ��ģʽ)
    {
        Scan_Wtime = 5;            //��Ҫ��ʱ��ʱ��
        TIM4_SetARR(9999);          //����1S��ʱ�ж�
    }


    if((Re1&&waittime)||(Re3&&waittime)||(Re2&&waittime))		//��Ҫ�ȴ�Ӧ��
    {
        while(--waittime)	//�ȴ�����ʱ
        {
            if(BT_Scan_mode)    //ɨ��ģʽ
            {
                res=KEY_Scan(0);//������һ��
                if(res==WKUP_PRES)return 2;
            }
            delay_ms(10);

            if(USART2_RX_STA&0X8000)//���յ��ڴ���Ӧ����
            {
                printf("ATREV<-- ");
                printf((const char*)USART2_RX_BUF,"\r\n"); //�յ���ģ�鷴����Ϣ

                if (NB_check_cmd(Re1))
                {
                    return 1;
                }
                if (NB_check_cmd(Re2))
                {
                    return 2;
                }
                if (NB_check_cmd(Re3))
                {
                    return 3;
                }
                USART2_RX_STA=0;
            }
        }
    }
    return res;
} 

//����NB�������ݣ���������ģʽ��ʹ�ã�
//request:�ڴ����������ַ���
//waittimg:�ȴ�ʱ��(��λ��10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8 NB_wait_request(u8 *request ,u16 waittime)
{
    u8 res = 1;
    u8 key;
    if(request && waittime)
    {
        while(--waittime)
        {
            key=KEY_Scan(0);
            if(key==WKUP_PRES) return 2;//������һ��
            delay_ms(10);
            if(USART2_RX_STA &0x8000)//���յ��ڴ���Ӧ����
            {
                if(NB_check_cmd(request)) break;//�õ���Ч����
                USART2_RX_STA=0;
            }
        }
        if(waittime==0)res=0;
    }
    return res;
}

//��1���ַ�ת��Ϊ16��������
//chr:�ַ�,0~9/A~F/a~F
//����ֵ:chr��Ӧ��16������ֵ
u8 NB_chr2hex(u8 chr)
{
    if(chr>='0'&&chr<='9')return chr-'0';
    if(chr>='A'&&chr<='F')return (chr-'A'+10);
    if(chr>='a'&&chr<='f')return (chr-'a'+10);
    return 0;
}
//��1��16��������ת��Ϊ�ַ�
//hex:16��������,0~15;
//����ֵ:�ַ�
u8 NB_hex2chr(u8 hex)
{
    if(hex<=9)return hex+'0';
    if(hex>=10&&hex<=15)return (hex-10+'A');
    return '0';
}

//ͨ����λ�����ư��������LED 3��
void BC95_RECData(void)
{
    if(strstr((const char*)USART2_RX_BUF,(const char*)"55010066"))
    {
        GPIO_SetBits(GPIOB,GPIO_Pin_5);
        GPIO_SetBits(GPIOB,GPIO_Pin_6);
        GPIO_SetBits(GPIOB,GPIO_Pin_7);
    }
    else if(strstr((const char*)USART2_RX_BUF,(const char*)"55010166"))
    {
        GPIO_ResetBits(GPIOB,GPIO_Pin_5);
        GPIO_ResetBits(GPIOB,GPIO_Pin_6);
        GPIO_ResetBits(GPIOB,GPIO_Pin_7);

    }
}
