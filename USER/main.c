#include "stm32f10x.h"
#include "st7290.h"
#include "delay.h"


 int main(void)
 {	
			SystemInit(); //72MHz
			delay_init();
			Lcd_Init();
	 		hanzi_Disp(1,0,"LCD12864液晶模块");
			delay_us(10000);
			hanzi_Disp(2,0,"ST7290串行口驱动");
			delay_us(10000);
			hanzi_Disp(3,0,"屏内汉字调用显示");
			delay_us(10000);
			hanzi_Disp(4,0,"希望能够帮到你啦");
			delay_us(10000);

			//Lcd_Init();
			//hanzi_Disp(2,2,"屏点检测");
			delay_us(10000);
			Lcd_Fill(0x00); 
			delay_us(10000);

			while(1)
			{
			  GPIO_ResetBits(GPIOA,GPIO_Pin_0);
			  delay_ms(1000);
			  GPIO_SetBits(GPIOA,GPIO_Pin_0);	
			}
 
}

