#ifndef _ST7290_h_          
#define _ST7290_h_

void Lcd_Init(void);

void SendByte(unsigned char Dbyte);
void Lcd_Write_Cmd(unsigned char Cbyte);
void Lcd_Write_Data(unsigned char Dbyte);

void hanzi_Disp(unsigned char x,unsigned char y,unsigned char *s);
void LCD_Set_XY(unsigned char x,unsigned char y); 

void Lcd_Fill(unsigned char color); 
#endif

