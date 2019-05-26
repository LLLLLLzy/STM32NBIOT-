/******
湖畔科技云平台地址；http://www.nnhpiot.com/
板子购买地址：https://item.taobao.com/item.htm?id=569466220378
网店：https://lcwl.taobao.com/
微信公众号：如需要源代码，可以关注“南宁湖畔科技”微信公众号。微信号：NNHPTCH
或者扫描下面的二维码。发送：BC35DHT11即可获取到单片机源代码。
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

// ADC1转换的电压值通过MDA方式传到SRAM
extern __IO uint16_t ADC_ConvertedValue[macNOFCHANEL];
//ErrorStatus NB_ret(void);  //重启模块
ErrorStatus NB_init(void);
__IO u8 commandAT[255];     // 存放指令，包含ip地址和端口和长度
__IO u8 locationLen[255];   //存放数据长度

__IO u8 location[255];          //存放数据经纬度用来发送
__IO u8 locationxianshi[255];   //存放数据经纬度用来显示

__IO u8 locationHex[255];   //存放转换成16进制后的数据
__IO u8 HTTPTCPData[255];   //存放发送信息就是最后发送的命令里面的数据

__IO u8 COM1Hex[255];       //存放串口1转换成16进制后的数据，进行透传的十六进制
__IO u8 COM1Data[255];      //存放串口1接收数据，透传的字符串

__IO u8 cardIMSI[80];       //卡号存放
__IO u8 signalCSQ[80];      //信号存放
char *Mystr="ss";
int id=0;                   //每次发送的数据包
DHT11_Data_TypeDef DHT11_Data;
#define DEV_ID      "hpwlwde58591b"							//需要定义为用户自己的参数
#define DEV_KEY     "8ce6a4e89013cc90"				//需要定义为用户自己的参数
//#define HEADDATA "7777772e637367736d2e636f6d\r\n"
u8 HTTP_PostPkt(char *pkt,char *devid,char *devkey,char *val1, char *val2);
int main(void)
{
	char sum=0;
    float adc1;            //AD采集变量
	char HTTP_Buf[400];     //HTTP报文缓存区
	char HTTP_BufHEX[450];     //HTTP报文缓存区
	char HTTP_BufSend[500];     //HTTP报文缓存区
	char commandAT[100];     //HTTP报文缓存区
	char tempStr[5];        //???????
    char humiStr[5];        //???????
		u8 http_len=0;					//HTTP组包数据长度
	
    NVIC_Configuration(); 	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
    delay_init();	    	//延时函数初始化
    uart_init(9600);	 	//串口初始化为9600
    delay_ms(100);
    printf("串口一初始化成功\r\n");//拨号

    LED_Init();		  		//初始化与LED连接的硬件接口
    ADCx_Init();            // ADC 初始化


    delay_ms(100);
    USART2_Init(9600);	//与GSM模块通信
    uart3_init(9600);
    delay_ms(100);

    u2_printf("串口二初始化成功\r\n");//拨号
    Usart_SendString(USART3,"串口三初始化成功\r\n");//打印串口三测试数据
	DHT11_Init ();
    OLED_Init();				//初始化OLED
    OLED_Clear();

    while(1)
    {
        if (NB_init() == ERROR) continue;	//与服务器建立HTTP链接;
        printf("\r\n=====已连接服务器====\r\n");
        while(1)
        {
			ClearRAM((u8*)HTTP_BufSend,500); //清空HTTPTCPData数据包缓存，最后发送的
			ClearRAM((u8*)HTTP_Buf,500);    //清空location数据包缓存，保存经纬度的数据
			ClearRAM((u8*)humiStr,255);   //清空commandAT数据包缓存，发送命令包含ip端口和长度
			ClearRAM((u8*)tempStr,255); //清空locationHex数据包缓存，转换为hex 需要发送的真实数据
			ClearRAM((u8*)HTTP_BufHEX,500);//清空locationHex数据包缓存，用来液晶屏显示的数据
			DHT11_Read_TempAndHumidity (&DHT11_Data );
			if( DHT11_Read_TempAndHumidity ( & DHT11_Data ) == SUCCESS)
			{
				printf("\r\n读取DHT11成功!\r\n\r\n湿度为%d.%d ％RH ，温度为 %d.%d℃ \r\n",\
				DHT11_Data.humi_int,DHT11_Data.humi_deci,DHT11_Data.temp_int,DHT11_Data.temp_deci);
			}
			printf("\r\n读取DHT11成功!\r\n\r\n湿度为%d.%d ％RH ，温度为 %d.%d℃ \r\n",\
			DHT11_Data.humi_int,DHT11_Data.humi_deci,DHT11_Data.temp_int,DHT11_Data.temp_deci);
			sprintf(tempStr, "%d", DHT11_Data.temp_int);
			sprintf(humiStr, "%d", DHT11_Data.humi_int);
			if ((NB_send_cmd((u8 *)"AT+NSOCR=STREAM,6,56000,1\r\n","\r\nOK","NULL","NULL",200)))         //调用发送函数
			{
				delay_ms(900);
				USART2_RX_STA=0;     //清空串口2的模块反馈
				if((NB_send_cmd((u8 *)"AT+NSOCO=1,182.92.2.22,80\r\n","\r\nOK","NULL","NULL",2000)))         //调用发送函数
				{
					USART2_RX_STA=0;     //清空串口2的模块反馈
					sum++;
					http_len=HTTP_PostPkt(HTTP_Buf,DEV_ID,DEV_KEY,tempStr,humiStr);
					strncpy((char*)(HTTP_Buf+strlen((char*)HTTP_Buf)),"\r\n",2);
					Str2Hex((char*)HTTP_Buf,(char*)HTTP_BufHEX);//将经纬度转换为转16进制，方便发送到服务器
					delay_ms(900);
					delay_ms(900);
					sprintf((char*)(HTTP_BufSend),"%s,%d,%s","AT+NSOSD=1",strlen((char *)HTTP_Buf),(char *)HTTP_BufHEX); //组成一个命令 包含命令 长度 经纬度和其他的数据的十六进制
					NB_send_cmd((u8 *)HTTP_BufSend,"OK","NULL","NULL",2000);	//发送数据到服务器
					printf("HTTP_BufSend=%s\r\n",(char*)HTTP_BufSend);  //打印我们需要发送的数据 看下是否是对的
					delay_ms(900);
					USART2_RX_STA=0;     //清空串口2的模块反馈
				}	
			}
			else
			{
				NB_send_cmd((u8 *)"AT+NSOCL=1\r\n","OK","NULL","NULL",200);
				delay_ms(900);
				USART2_RX_STA=0;     //清空串口2的模块反馈
			}
        }
    }
}

ErrorStatus NB_init(void)
{
    u8 data=0,ret=0;
    u8 err=0;
    USART2_RX_STA=0;

    delay_ms(2000);                                                 //等待系统启动
    if(NB_send_cmd("AT","OK","NULL","NULL",1000))err|=1<<0;         //检测是否应答AT指令

    USART2_RX_STA=0;
    if(NB_send_cmd("AT+CMEE=1","OK","NULL","NULL",1000))err|=1<<1;  //允许错误值 /* Use AT+CMEE=1 to enable result code and use numeric values */

    USART2_RX_STA=0;
    NB_send_cmd("AT+CGSN=1\r\n","OK","NULL","NULL",1000);

    USART2_RX_STA=0;
    memset(USART2_RX_BUF,0,USART2_MAX_RECV_LEN);
    NB_send_cmd("AT+NBAND?\r\n","NBAND","OK","NULL",1000); //设置频段号
    printf((const char*)USART2_RX_BUF,"\r\n"); //收到的模块反馈信息,直接反馈了ret信息没有被清空

    if(strstr((char*)USART2_RX_BUF,"+NBAND:8"))
	{
		NB_send_cmd("AT+NBAND=8\r\n","OK","OK","NULL",1000); //设置频段号
		OLED_ShowString(4,4,"BAND:8 REG[..]");//显示移动联通;
	}
	if(strstr((char*)USART2_RX_BUF,"+NBAND:5"))
	{
		NB_send_cmd("AT+NBAND=5\r\n","OK","OK","NULL",1000); //设置频段号
		OLED_ShowString(4,4,"BAND:5 REG[..]");//显示移动联通;
	}
														 
    USART2_RX_STA=0;
    NB_send_cmd("AT+NBAND?\r\n","+NBAND:7","NULL","NULL",1000); 	//获取频段号 参数不要改 增加检测时间


    USART2_RX_STA=0;
    if(NB_send_cmd("AT+CIMI","OK","NULL","NULL",1000))err|=1<<3;	//获取卡号，类似是否存在卡的意思，比较重要。
    ClearRAM((u8*)cardIMSI,80);
    sprintf((char *)cardIMSI,"%s",(char*)(&USART2_RX_BUF[2]));
    printf("我是卡号：%s\r\n",cardIMSI);//发送命令

    if(!strstr((char*)cardIMSI ,"ERROR"))
        OLED_ShowString(4,4,"NBSIMCARD [OK]");//显示卡状态OK
    else
        OLED_ShowString(4,4,"NBSIMCARD [NO]");

    USART2_RX_STA=0;                        //先关闭PDP
//    memset(USART2_RX_BUF,0,USART2_MAX_RECV_LEN);
//    NB_send_cmd("AT+CGATT=0\r\n","+CGATT:0","NULL","NULL",1000);
    delay_ms(500);
/*
    USART2_RX_STA=0;
                            //查询PDP的激活情况
    NB_send_cmd("AT+CGATT?\r\n","+CGATT: 1","OK","NULL",1000);
*/
    USART2_RX_STA=0;                        //显示信号
    NB_send_cmd("AT+CSQ\r\n","+CSQ","NULL","NULL",1000);
    if(strstr((const char*)USART2_RX_BUF,(const char*)"+CSQ:"))
    {
        ClearRAM((u8*)signalCSQ,80);            //清空信号数组
        strncpy((char*)(signalCSQ+strlen((char*)signalCSQ)),(char*)(&USART2_RX_BUF[3]),6); //拷贝信号的6个字节
        OLED_ShowString(4,6,(u8 *)signalCSQ);      //显示信号质量
        printf("我是信号质量：%s\r\n",USART2_RX_BUF);  //打印信号质量 所有字符都打印
    }
    USART2_RX_STA=0;                        
//    data=NB_send_cmd((u8 *)HTTPTCPData,"OK","NULL","NULL",1000);	//发送数据到服务器


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

//重启NB模块
ErrorStatus NB_ret(void)
{
    NB_send_cmd("AT+NRB\r\n","OK","NULL","NULL",2000);
    USART2_RX_STA=0;
    delay_ms(5000);
    return SUCCESS;
}


