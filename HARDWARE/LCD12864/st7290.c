#include "stm32f10x.h"	
#include "st7290.h"
#include "delay.h"
/****************************************
12864的驱动采用串行接法
****************************************/
typedef unsigned char uchar;
typedef unsigned int  unit;  //宏定义

/*库函数方式
#define   LCD_PSB_0       GPIO_ResetBits(GPIOB,GPIO_Pin_12) //串口PSB拉低
#define   LCD_RS_1	      GPIO_SetBits(GPIOB,GPIO_Pin_15) //片延RS拉高
#define   LCD_SCLK_0      GPIO_ResetBits(GPIOB,GPIO_Pin_13)  //EN串口为时钟
#define   LCD_SCLK_1      GPIO_SetBits(GPIOB,GPIO_Pin_13)
#define   LCD_SID_0       GPIO_ResetBits(GPIOB,GPIO_Pin_14) 
#define   LCD_SID_1       GPIO_SetBits(GPIOB,GPIO_Pin_14)//RW串口为数据，并口为读写
*/
#define   LCD_PSB_0       GPIOB->ODR &=~(1<<12)  //串并转换也可以直接接地
#define   LCD_RS_1	      GPIOB->ODR |= (1<<15)		//也可以直接上拉。片选
#define   LCD_SCLK_0      GPIOB->ODR &=~(1<<13)
#define   LCD_SCLK_1      GPIOB->ODR |= (1<<13)	//就是EN，串行中变为SCLK
#define   LCD_SID_0       GPIOB->ODR &=~(1<<14) 
#define   LCD_SID_1       GPIOB->ODR |= (1<<14) //就是RW在串行中就变为SID
/************************************************LCD功能初始化指令-----------无用，仅参考
#define CLEAR_SCREEN 0x01   //清屏指令：清屏且AC值为00H
#define AC_INIT   0x02   //将AC设置为00H。且游标移到原点位置
#define CURSE_ADD  0x06   //设定游标移到方向及图像整体移动方向（默认游标右移，图像整体不动）
#define FUN_MODE  0x30   //工作模式：8位基本指令集
#define DISPLAY_ON  0x0c   //显示开,显示游标，且游标位置反白
#define DISPLAY_OFF  0x08   //显示关
#define CURSE_DIR  0x14   //游标向右移动:AC=AC+1
#define SET_CG_AC  0x40   //设置AC，范围为：00H~3FH
#define SET_DD_AC  0x80
#define MAP_ON        0x36   //图形显示开
#define MAP_OFF       0x34   //图形显示关
*******************************************************************/

void Lcd_Init(void) 
{ /*  库函数方式
  GPIO_InitTypeDef  GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);//使能端口时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);	//根据设定参数初始化GPIO
  GPIO_SetBits(GPIOA,GPIO_Pin_0);//输出高
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;//端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIO
  GPIO_SetBits(GPIOB,GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);//输出高
	*/
	RCC->APB2ENR |= 1<<2; //使能PA时钟
	RCC->APB2ENR |= 1<<3; //使能PB时钟
	GPIOA->CRL &= 0XFFFFFFF0;
	GPIOA->CRL |= 0X00000003; //PA.0挽推输出
	GPIOA->ODR |= 1<<0;       //PA.0输出1
	GPIOB->CRH &= 0X0000FFFF;
	GPIOB->CRH |= 0X33330000; //PB.12/13/14/15挽推输出
	GPIOB->ODR |= (1<<12)|(1<<13)|(1<<14)|(1<<15);//输出1	
	
  LCD_PSB_0; //一直拉低，用串口方式驱动 （可直接接地）
  LCD_RS_1;  //CS一直拉高，使能液晶（可直接接VCC）

	Lcd_Write_Cmd(0x30); 
	delay_us(10000);
	Lcd_Write_Cmd(0x01);
	delay_us(10000);
	Lcd_Write_Cmd(0x02);
	delay_us(10000);
	Lcd_Write_Cmd(0x06);
	delay_us(10000);
	Lcd_Write_Cmd(0x0c);
	delay_us(10000);
	Lcd_Write_Cmd(0x34); //图形开启（不显示）
	delay_us(10000);
	Lcd_Fill(0x00);    //清空屏内RAM
	delay_us(10000);
	Lcd_Write_Cmd(0x36); //图形开启（开显示）
	delay_us(10000);
}

void SendByte(uchar Dbyte)  
{
     uchar i;
     for(i=8;i>0;i--)
     {
     if (Dbyte&0x80)  LCD_SID_1;
   	   else LCD_SID_0; 
     LCD_SCLK_1;  //如果显示有问题，则需在此后加延迟
		 delay_us(1); //72MHZ速度太快，M0的48MHZ就无需加
     LCD_SCLK_0;
     Dbyte<<=1;
     }
}
void Lcd_Write_Cmd(uchar Cbyte )
{
     //LCD_RS_1;
     SendByte(0xf8);              //11111,RW(0),RS(0),0
     SendByte(0xf0&Cbyte);        
     SendByte(0xf0&Cbyte<<4);   
     //LCD_RS_0;
}

void Lcd_Write_Data(uchar Dbyte )
{
     //LCD_RS_1; 
     SendByte(0xfa);              //11111,RW(0),RS(1),0
     SendByte(0xf0&Dbyte);        
     SendByte(0xf0&Dbyte<<4);  
     //LCD_RS_0; 
}

void LCD_Set_XY(uchar x,uchar y) 
{
    unsigned char k = 0;   
	switch(x)
	{
	 case 1: 
	   		k = 0x80 + y;
	    		break; 	
     case 2: 
			k = 0x90 + y;
				break;    
	 case 3: 
			k = 0x88 + y;
				break;	 
     case 4: 
			k = 0x98 + y;
				break; 	
	 default:
	 		k = 0x80 + y;
	}
    Lcd_Write_Cmd(k);
}


void hanzi_Disp(unsigned char x,unsigned char y,unsigned char *s)
{ 
	Lcd_Write_Cmd(0x30); //进入标准模式
	LCD_Set_XY(x,y);
	while (*s)  
	{
		Lcd_Write_Data(*s);
		s++;
	}
	Lcd_Write_Cmd(0x36); //返回图形模式
}


void Lcd_Fill(uchar color) //填充屏内RAM
{  
 uchar x,y,i;
 Lcd_Write_Cmd(0x34);  
    for(i=0;i<9;i=i+8)   
 for(y=0;y<0x20;y++)
 {     
  for(x=0;x<8;x++)
       { 
        Lcd_Write_Cmd(y+0x80);       
        Lcd_Write_Cmd(x+0x80+i);      
        Lcd_Write_Data(color);   
        Lcd_Write_Data(color);   
       }
 } 
Lcd_Write_Cmd(0x36);
}


