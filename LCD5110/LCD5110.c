#include "LCD5110.h"
#include "Pixel_EN.h"
#include "delay.h"
#include "string.h" 
#include "stm32f10x_dma.h"
#include "stm32f10x_spi.h"
//#include "Pixel_CH.h"
static u8 lcd_buf[84*6] ;
DMA_InitTypeDef DMA_initstructure ;
/*******************************************************************************
* Function Name  : LCD_Init
* Description    : LCD��ʼ��
* Input          : None	
* Output         : None
* Return         : None
*******************************************************************************/
void LCD5110_Init(void)
{
	  LCD5110_IO_Configuration();
	  //��λLCD_�͵�ƽ����
	  LCD_RST_L();		
	  delay_ms(5);
   	LCD_RST_H();
	  //��λ���нӿ�_�ߵ�ƽ����
	  LCD_CE_H(); 
	  delay_ms(5);
	  LCD_CE_L(); 
	  //����LCD
    LCD_Send(0x21, DC_CMD);	//ʹ����չ��������LCDģʽ
    LCD_Send(0xC8, DC_CMD);	//����ƫ�õ�ѹ
    LCD_Send(0x06, DC_CMD);	//�¶�У��
    LCD_Send(0x13, DC_CMD); //1:48
    LCD_Send(0x20, DC_CMD);	//ʹ�û�������
    LCD_Send(0x0C, DC_CMD);	//�趨��ʾģʽ����ת��ʾ
}
/*******************************************************************************
* Function Name  : LCD_SetContrast
* Description    : ����LCD�Աȶ�(�Աȶȷ�Χ: 0~127)
* Input          : u8 contrast
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_SetContrast(u8 contrast)
{
    LCD_Send(0x21, DC_CMD);
    LCD_Send(0x80 | contrast, DC_CMD);
    LCD_Send(0x20, DC_CMD);
}
/*******************************************************************************
* Function Name  : LCD_SetXY
* Description    : ����LCD��ǰ����
* Input          : u8 X, u8 Y	
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_SetXY(u8 X, u8 Y)
{
	if (X>13) X = 13;
	if (Y>5) Y = 5;
	X *= 6;
	//Y *= 8;
	LCD_Send(0x80 | X, DC_CMD);    // �� 
	delay_ms(10)         ;  
	LCD_Send(0x40 | Y, DC_CMD);    // ��
}
/*******************************************************************************
* Function Name  : LCD_Clear
* Description    : LCD����
* Input          : None	
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Clear(void)
{
	u16 i;
  LCD_Send(0x80, DC_CMD);
	delay_ms(5);
	LCD_Send(0x40, DC_CMD);
	for(i=0; i<504; i++)
	LCD_Send(0, DC_DATA);	  
}
/*******************************************************************************
* Function Name  : LCD_Send
* Description    : ��LCD��������
* Input          : u8 data, DCType dc
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Send(u8 data, DCType dc)
{
	u8 i=8;
	#if USESPI
  while( SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE ) == RESET );
	if (dc==DC_CMD)
		LCD_DC_CMD();	//��������
	else
		LCD_DC_DATA();//��������
    SPI_I2S_SendData(SPI1, data);
    //�ȴ�������ɣ����� LCD_SET_XY ��������
    while( SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE ) == RESET );
	#else
   	if (dc==DC_CMD)
		LCD_DC_CMD();	//��������
	else
		LCD_DC_DATA();//��������
	while (i--)
	{
		LCD_CLK_L();
		if(data&0x80)
			LCD_MOSI_H();	/* MSB first */
		else 
			LCD_MOSI_L();
		LCD_CLK_H();
		data <<= 1;		/* Data shift */
	}
	#endif
}
/*******************************************************************************
* Function Name  : LCD_Write_Char
* Description    : ��LCDдһ��Ӣ���ַ�
* Input          : u8 ascii
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Write_Char(u8 ascii)
{
    u8 n;
    ascii -= 32; //ASCII��-32
    for (n=0; n<6; n++)
		LCD_Send(Font6x8[ascii][n], DC_DATA);
}
/*******************************************************************************
* Function Name  : LCD_Write_Char
* Description    : ��LCDдһ������
* Input          : u8 ascii
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Write_Hanzi(u8 X, u8 Y,u8 GBK)
{
    u8 n;
     LCD_SetXY(X,Y); 
     for (n=0; n<12; n++)
	      {      
		       LCD_Send(Font12x16_chinese[GBK][n], DC_DATA);
		    }
	 
	  delay_ms(10);
		LCD_SetXY(X,Y+1); 
    for (n=12; n<24; n++)
	    {      
		         LCD_Send(Font12x16_chinese[GBK][n], DC_DATA);
		  }
			
}
/*******************************************************************************
* Function Name  : LCD_Write_EnStr
* Description    : ��LCDдӢ���ַ���
* Input          : u8 ascii
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Write_EnStr(u8 X, u8 Y, u8* s)
{
    u8 * str = s;
    int i = 0;
    int lcd_index =0;
    if(str == 0 )
    {
        return ;
    }
    //�ȴ��ϴ�DMA�������
    while(DMA_GetCurrDataCounter(DMA1_Channel3)) ;  
    LCD_SetXY(X,Y);
    while(*str)
    {
        //������ʾ���ݵ�������
        for(i=0;i<6;i++)
        {
            lcd_buf[lcd_index ++ ] = Font6x8[*str - 32][i];
        }
					//memcpy(lcd_buf+6*str,&Font6x8[*str - 32][0],6);	
        str ++ ;
    }
    lcd_buf[lcd_index ++ ] = 0 ; // lcd_index ++ �෢��һ��0�������һ���ַ���ȱ��һ������
//    #if USESPI
//			DMA_initstructure.DMA_BufferSize = lcd_index ;
//		 
//			DMA_Cmd(DMA1_Channel3, DISABLE); 
//			DMA_Init( DMA1_Channel3, &DMA_initstructure) ;
//			DMA_Cmd(DMA1_Channel3, ENABLE);
//			LCD_DC_DATA();  
//			SPI_I2S_DMACmd( SPI1, SPI_I2S_DMAReq_Tx, ENABLE) ;
//    #else
    for(i = 0 ;i<lcd_index ;i++)
    {
        LCD_Send(lcd_buf[i], DC_DATA);
    }
//    #endif
}
/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD5110_IO_Configuration(void)
{  	 
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PA,PC�˿�ʱ��
	/*LCD5110_LCD_CTRL_PORT*/
 	GPIO_InitStructure.GPIO_Pin =LCD_RST;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 	GPIO_Init(GPIO_LCD_RST_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =LCD_CE;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 	GPIO_Init(GPIO_LCD_CE_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =LCD_DC;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 	GPIO_Init(GPIO_LCD_DC_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =LCD_CLK	;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 	GPIO_Init(GPIO_SCLK_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =LCD_MOSI;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 	GPIO_Init(GPIO_SDIN_PORT, &GPIO_InitStructure);
}



