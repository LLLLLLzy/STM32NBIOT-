#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "sys.h"
#include "led.h"
#include "key.h"
#include "usart.h"	
//#include "sim800c.h"
#include "string.h"  
/**
  * @brief   组HTTP 协议包
  * @param   pkt   报文缓存指针
  *	@param 	 devid 设备ID，定义在main.c文件中，需要根据自己的设备修改
	* @param   devkey 设备密钥定义在Main.c文件中，需要根据自己的设备修改
	*	@param 	 val1  上传数据值1
  *	@param 	 val2  上传数据值2
  * @retval  返回值:整个包的长度
  */
u8 HTTP_PostPkt(char *pkt,char *devid,char *devkey,char *val1, char *val2)
{
//    char dataBuf[100] = {0};
//    *pkt = 0;

//				sprintf(dataBuf, "/lat/%s/lng/%s", val1, val2);     //采用分割字符串格式:type = 5

//        strcat(pkt, "POST http://api.nnhpiot.com/v1/gpsdata/");
//				strcat(pkt, devid);
//				strcat(pkt, "/");
//				strcat(pkt, devkey);

// 				strcat(pkt, dataBuf);
//				strcat(pkt, "\r\n");
 
//    return strlen(pkt);
	 *pkt = 0;
	
		strcat(pkt, "POST http://api.nnhpiot.com/v1/Uploadtemp/");		
		strcat(pkt, devid);
		strcat(pkt, "/");
		strcat(pkt, devkey);
		strcat(pkt, "/temperature/");
		strcat(pkt, val1);
		strcat(pkt, "/humidity/");
		strcat(pkt, val2);		
		strcat(pkt, "\r\n");
    return strlen(pkt);
}
