#include "nbiot.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h"   	 
#include "delay.h"
#include "key.h"	 	 	 	 	 
#include "string.h"    
#include "usart2.h" 

u8 Scan_Wtime = 0;//保存扫描需要的时间
u8 BT_Scan_mode=0;//扫描设备模式标志
char *extstrx;
extern char  RxBuffer1[100],RxCounter;
BC95 BC95_Status;
__IO u8 jie[255];   //存放接收信息

//usmart支持部分 
//将收到的AT指令应答数据返回给电脑串口
//mode:0,不清零USART2_RX_STA;
//     1,清零USART2_RX_STA;
void sim_at_response(u8 mode)
{
    if(USART2_RX_STA&0X8000)		//接收到一次数据了
    {
        USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;//添加结束符
        printf("%s",USART2_RX_BUF);	//发送到串口
        if(mode)USART2_RX_STA=0;
    }
}
//////////////////////////////////////////////////////////////////////////////////
//ATK-NB 各项测试(拨号测试、短信测试、GPRS测试、蓝牙测试)共用代码

//NB发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//其他,期待应答结果的位置(str的位置)
u8* NB_check_cmd(u8 *str)
{
    char *strx=0;
    if(USART2_RX_STA&0X8000)		//接收到一次数据了
    {
        USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;//添加结束符
        strx=strstr((const char*)USART2_RX_BUF,(const char*)str);
    }
    return (u8*)strx;
}
//向NB发送命令
//cmd:发送的命令字符串(不需要添加回车了),当cmd<0XFF的时候,发送数字(比如发送0X1A),大于的时候发送字符串.
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 NB_send_cmd(u8 *cmd,u8 *Re1,u8 *Re2,u8 *Re3,u16 waittime)
{

    u8 res=0;
    USART2_RX_STA=0;
    if((u32)cmd<=0XFF)
    {
        while(DMA1_Channel7->CNDTR!=0);	//等待通道7传输完成
        USART2->DR=(u32)cmd;
    }else
    {
        u2_printf("%s\r\n",cmd);    //发送命令
        printf("ATSEND--> %s\r\n",cmd);       //打印调试
    }

    if(waittime==500)              //11s后读回串口数据(扫描模式)
    {
        Scan_Wtime = 5;            //需要定时的时间
        TIM4_SetARR(9999);          //产生1S定时中断
    }


    if((Re1&&waittime)||(Re3&&waittime)||(Re2&&waittime))		//需要等待应答
    {
        while(--waittime)	//等待倒计时
        {
            if(BT_Scan_mode)    //扫描模式
            {
                res=KEY_Scan(0);//返回上一级
                if(res==WKUP_PRES)return 2;
            }
            delay_ms(10);

            if(USART2_RX_STA&0X8000)//接收到期待的应答结果
            {
                printf("ATREV<-- ");
                printf((const char*)USART2_RX_BUF,"\r\n"); //收到的模块反馈信息

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

//接收NB返回数据（蓝牙测试模式下使用）
//request:期待接收命令字符串
//waittimg:等待时间(单位：10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 NB_wait_request(u8 *request ,u16 waittime)
{
    u8 res = 1;
    u8 key;
    if(request && waittime)
    {
        while(--waittime)
        {
            key=KEY_Scan(0);
            if(key==WKUP_PRES) return 2;//返回上一级
            delay_ms(10);
            if(USART2_RX_STA &0x8000)//接收到期待的应答结果
            {
                if(NB_check_cmd(request)) break;//得到有效数据
                USART2_RX_STA=0;
            }
        }
        if(waittime==0)res=0;
    }
    return res;
}

//将1个字符转换为16进制数字
//chr:字符,0~9/A~F/a~F
//返回值:chr对应的16进制数值
u8 NB_chr2hex(u8 chr)
{
    if(chr>='0'&&chr<='9')return chr-'0';
    if(chr>='A'&&chr<='F')return (chr-'A'+10);
    if(chr>='a'&&chr<='f')return (chr-'a'+10);
    return 0;
}
//将1个16进制数字转换为字符
//hex:16进制数字,0~15;
//返回值:字符
u8 NB_hex2chr(u8 hex)
{
    if(hex<=9)return hex+'0';
    if(hex>=10&&hex<=15)return (hex-10+'A');
    return '0';
}

//通过上位机控制板子上面的LED 3个
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
