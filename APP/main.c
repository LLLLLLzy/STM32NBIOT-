/******
���ϿƼ���ƽ̨��ַ��http://www.nnhpiot.com/
���ӹ����ַ��https://item.taobao.com/item.htm?id=569466220378
���꣺https://lcwl.taobao.com/
΢�Ź��ںţ�����ҪԴ���룬���Թ�ע���������ϿƼ���΢�Ź��ںš�΢�źţ�NNHPTCH
����ɨ������Ķ�ά�롣���ͣ�BC35DHT11���ɻ�ȡ����Ƭ��Դ���롣
*****/

#include "delay.h"
#include "sys.h"
#include "led.h"
#include "key.h"
#include "usart.h"	
#include "nbiot.h"
#include "string.h"    
#include "usart2.h" 
#include "usart3.h"	
#include "misc.h"  
#include "DHT11.h"

#include "ADCx.h"
#include "beep.h"
#include "math.h" 
#include "HEXSTR.h"

#include "oled.h"
#include "bmp.h"


#define MAX_CONVERTD  4095
#define VREF      3300

// ADC1ת���ĵ�ѹֵͨ��MDA��ʽ����SRAM
extern __IO uint16_t ADC_ConvertedValue[macNOFCHANEL];
//ErrorStatus NB_ret(void);  //����ģ��
ErrorStatus NB_init(void);
__IO u8 commandAT[255];     // ���ָ�����ip��ַ�Ͷ˿ںͳ���
__IO u8 locationLen[255];   //������ݳ���

__IO u8 location[255];          //������ݾ�γ����������
__IO u8 locationxianshi[255];   //������ݾ�γ��������ʾ

__IO u8 locationHex[255];   //���ת����16���ƺ������
__IO u8 HTTPTCPData[255];   //��ŷ�����Ϣ��������͵��������������

__IO u8 COM1Hex[255];       //��Ŵ���1ת����16���ƺ�����ݣ�����͸����ʮ������
__IO u8 COM1Data[255];      //��Ŵ���1�������ݣ�͸�����ַ���

__IO u8 cardIMSI[80];       //���Ŵ��
__IO u8 signalCSQ[80];      //�źŴ��
char *Mystr="ss";
int id=0;                   //ÿ�η��͵����ݰ�
DHT11_Data_TypeDef DHT11_Data;
#define DEV_ID      "hpwlwde58591b"							//��Ҫ����Ϊ�û��Լ��Ĳ���
#define DEV_KEY     "8ce6a4e89013cc90"				//��Ҫ����Ϊ�û��Լ��Ĳ���
//#define HEADDATA "7777772e637367736d2e636f6d\r\n"
u8 HTTP_PostPkt(char *pkt,char *devid,char *devkey,char *val1, char *val2);
int main(void)
{
	char sum=0;
    float adc1;            //AD�ɼ�����
	char HTTP_Buf[400];     //HTTP���Ļ�����
	char HTTP_BufHEX[450];     //HTTP���Ļ�����
	char HTTP_BufSend[500];     //HTTP���Ļ�����
	char commandAT[100];     //HTTP���Ļ�����
	char tempStr[5];        //???????
    char humiStr[5];        //???????
		u8 http_len=0;					//HTTP������ݳ���
	
    NVIC_Configuration(); 	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
    delay_init();	    	//��ʱ������ʼ��
    uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600
    delay_ms(100);
    printf("����һ��ʼ���ɹ�\r\n");//����

    LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�
    ADCx_Init();            // ADC ��ʼ��


    delay_ms(100);
    USART2_Init(9600);	//��GSMģ��ͨ��
    uart3_init(9600);
    delay_ms(100);

    u2_printf("���ڶ���ʼ���ɹ�\r\n");//����
    Usart_SendString(USART3,"��������ʼ���ɹ�\r\n");//��ӡ��������������
	DHT11_Init ();
    OLED_Init();				//��ʼ��OLED
    OLED_Clear();

    while(1)
    {
        if (NB_init() == ERROR) continue;	//�����������HTTP����;
        printf("\r\n=====�����ӷ�����====\r\n");
        while(1)
        {
			ClearRAM((u8*)HTTP_BufSend,500); //���HTTPTCPData���ݰ����棬����͵�
			ClearRAM((u8*)HTTP_Buf,500);    //���location���ݰ����棬���澭γ�ȵ�����
			ClearRAM((u8*)humiStr,255);   //���commandAT���ݰ����棬�����������ip�˿ںͳ���
			ClearRAM((u8*)tempStr,255); //���locationHex���ݰ����棬ת��Ϊhex ��Ҫ���͵���ʵ����
			ClearRAM((u8*)HTTP_BufHEX,500);//���locationHex���ݰ����棬����Һ������ʾ������
			DHT11_Read_TempAndHumidity (&DHT11_Data );
			if( DHT11_Read_TempAndHumidity ( & DHT11_Data ) == SUCCESS)
			{
				printf("\r\n��ȡDHT11�ɹ�!\r\n\r\nʪ��Ϊ%d.%d ��RH ���¶�Ϊ %d.%d�� \r\n",\
				DHT11_Data.humi_int,DHT11_Data.humi_deci,DHT11_Data.temp_int,DHT11_Data.temp_deci);
			}
			printf("\r\n��ȡDHT11�ɹ�!\r\n\r\nʪ��Ϊ%d.%d ��RH ���¶�Ϊ %d.%d�� \r\n",\
			DHT11_Data.humi_int,DHT11_Data.humi_deci,DHT11_Data.temp_int,DHT11_Data.temp_deci);
			sprintf(tempStr, "%d", DHT11_Data.temp_int);
			sprintf(humiStr, "%d", DHT11_Data.humi_int);
			if ((NB_send_cmd((u8 *)"AT+NSOCR=STREAM,6,56000,1\r\n","\r\nOK","NULL","NULL",200)))         //���÷��ͺ���
			{
				delay_ms(900);
				USART2_RX_STA=0;     //��մ���2��ģ�鷴��
				if((NB_send_cmd((u8 *)"AT+NSOCO=1,182.92.2.22,80\r\n","\r\nOK","NULL","NULL",2000)))         //���÷��ͺ���
				{
					USART2_RX_STA=0;     //��մ���2��ģ�鷴��
					sum++;
					http_len=HTTP_PostPkt(HTTP_Buf,DEV_ID,DEV_KEY,tempStr,humiStr);
					strncpy((char*)(HTTP_Buf+strlen((char*)HTTP_Buf)),"\r\n",2);
					Str2Hex((char*)HTTP_Buf,(char*)HTTP_BufHEX);//����γ��ת��Ϊת16���ƣ����㷢�͵�������
					delay_ms(900);
					delay_ms(900);
					sprintf((char*)(HTTP_BufSend),"%s,%d,%s","AT+NSOSD=1",strlen((char *)HTTP_Buf),(char *)HTTP_BufHEX); //���һ������ �������� ���� ��γ�Ⱥ����������ݵ�ʮ������
					NB_send_cmd((u8 *)HTTP_BufSend,"OK","NULL","NULL",2000);	//�������ݵ�������
					printf("HTTP_BufSend=%s\r\n",(char*)HTTP_BufSend);  //��ӡ������Ҫ���͵����� �����Ƿ��ǶԵ�
					delay_ms(900);
					USART2_RX_STA=0;     //��մ���2��ģ�鷴��
				}	
			}
			else
			{
				NB_send_cmd((u8 *)"AT+NSOCL=1\r\n","OK","NULL","NULL",200);
				delay_ms(900);
				USART2_RX_STA=0;     //��մ���2��ģ�鷴��
			}
        }
    }
}

ErrorStatus NB_init(void)
{
    u8 data=0,ret=0;
    u8 err=0;
    USART2_RX_STA=0;

    delay_ms(2000);                                                 //�ȴ�ϵͳ����
    if(NB_send_cmd("AT","OK","NULL","NULL",1000))err|=1<<0;         //����Ƿ�Ӧ��ATָ��

    USART2_RX_STA=0;
    if(NB_send_cmd("AT+CMEE=1","OK","NULL","NULL",1000))err|=1<<1;  //�������ֵ /* Use AT+CMEE=1 to enable result code and use numeric values */

    USART2_RX_STA=0;
    NB_send_cmd("AT+CGSN=1\r\n","OK","NULL","NULL",1000);

    USART2_RX_STA=0;
    memset(USART2_RX_BUF,0,USART2_MAX_RECV_LEN);
    NB_send_cmd("AT+NBAND?\r\n","NBAND","OK","NULL",1000); //����Ƶ�κ�
    printf((const char*)USART2_RX_BUF,"\r\n"); //�յ���ģ�鷴����Ϣ,ֱ�ӷ�����ret��Ϣû�б����

    if(strstr((char*)USART2_RX_BUF,"+NBAND:8"))
	{
		NB_send_cmd("AT+NBAND=8\r\n","OK","OK","NULL",1000); //����Ƶ�κ�
		OLED_ShowString(4,4,"BAND:8 REG[..]");//��ʾ�ƶ���ͨ;
	}
	if(strstr((char*)USART2_RX_BUF,"+NBAND:5"))
	{
		NB_send_cmd("AT+NBAND=5\r\n","OK","OK","NULL",1000); //����Ƶ�κ�
		OLED_ShowString(4,4,"BAND:5 REG[..]");//��ʾ�ƶ���ͨ;
	}
														 
    USART2_RX_STA=0;
    NB_send_cmd("AT+NBAND?\r\n","+NBAND:7","NULL","NULL",1000); 	//��ȡƵ�κ� ������Ҫ�� ���Ӽ��ʱ��


    USART2_RX_STA=0;
    if(NB_send_cmd("AT+CIMI","OK","NULL","NULL",1000))err|=1<<3;	//��ȡ���ţ������Ƿ���ڿ�����˼���Ƚ���Ҫ��
    ClearRAM((u8*)cardIMSI,80);
    sprintf((char *)cardIMSI,"%s",(char*)(&USART2_RX_BUF[2]));
    printf("���ǿ��ţ�%s\r\n",cardIMSI);//��������

    if(!strstr((char*)cardIMSI ,"ERROR"))
        OLED_ShowString(4,4,"NBSIMCARD [OK]");//��ʾ��״̬OK
    else
        OLED_ShowString(4,4,"NBSIMCARD [NO]");

    USART2_RX_STA=0;                        //�ȹر�PDP
//    memset(USART2_RX_BUF,0,USART2_MAX_RECV_LEN);
//    NB_send_cmd("AT+CGATT=0\r\n","+CGATT:0","NULL","NULL",1000);
    delay_ms(500);
/*
    USART2_RX_STA=0;
                            //��ѯPDP�ļ������
    NB_send_cmd("AT+CGATT?\r\n","+CGATT: 1","OK","NULL",1000);
*/
    USART2_RX_STA=0;                        //��ʾ�ź�
    NB_send_cmd("AT+CSQ\r\n","+CSQ","NULL","NULL",1000);
    if(strstr((const char*)USART2_RX_BUF,(const char*)"+CSQ:"))
    {
        ClearRAM((u8*)signalCSQ,80);            //����ź�����
        strncpy((char*)(signalCSQ+strlen((char*)signalCSQ)),(char*)(&USART2_RX_BUF[3]),6); //�����źŵ�6���ֽ�
        OLED_ShowString(4,6,(u8 *)signalCSQ);      //��ʾ�ź�����
        printf("�����ź�������%s\r\n",USART2_RX_BUF);  //��ӡ�ź����� �����ַ�����ӡ
    }
    USART2_RX_STA=0;                        
//    data=NB_send_cmd((u8 *)HTTPTCPData,"OK","NULL","NULL",1000);	//�������ݵ�������


//    USART2_RX_STA=0;
//    if (data == 1 || data == 2 || data == 3 || ret==1)
//    {
//        printf("\r\n====data=%d=====\r\n",data);
        return SUCCESS;
//    }
//    else
//    {
//        return ERROR;
//    }
} 

//����NBģ��
ErrorStatus NB_ret(void)
{
    NB_send_cmd("AT+NRB\r\n","OK","NULL","NULL",2000);
    USART2_RX_STA=0;
    delay_ms(5000);
    return SUCCESS;
}


