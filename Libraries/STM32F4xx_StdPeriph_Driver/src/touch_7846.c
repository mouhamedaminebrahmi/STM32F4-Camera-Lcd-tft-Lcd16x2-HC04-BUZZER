#include "touch_7846.h"
#include "LCD.h"
#include "stm32f4xx.h"
#include <stdio.h>

Pen_Holder Pen_Point;
#define ABS(X)  ((X) > 0 ? (X) : -(X))  
unsigned int xxx;
unsigned int yyy;
#define LCD_REG      (*((volatile unsigned short *) 0x60000000))
unsigned char flag=0;
#define LCD_RAM      (*((volatile unsigned short *) 0x60020000)) 
uint16_t TextColor = 0x0000;
														
unsigned char SPI_WriteByte(u8 num)    
{  
  unsigned char Data = 0;
  while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)==RESET); 
  SPI_I2S_SendData(SPI2,num);   
  while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_RXNE)==RESET); 
  Data = SPI_I2S_ReceiveData(SPI2);

  return Data; 	 				   
} 	

void SpiDelay(unsigned int DelayCnt)
{
 unsigned int i;
 for(i=0;i<DelayCnt;i++);
}

u16 TPReadX(void)
{ 
   u16 x=0;
   T_CS();
   SpiDelay(10);
   SPI_WriteByte(0x90);
   SpiDelay(10);  
   x=SPI_WriteByte(0xFF);
   x<<=8;
   x+=SPI_WriteByte(0x0);	
   T_DCS(); 
   //SpiDelay(10);    
   x=x>>4;
x  = x & 0xFFfF; //fff
x=2047-x;
   return x;
}


u16 TPReadY(void)
{
  u16 y=0;
  T_CS();
  SpiDelay(10);
  SPI_WriteByte(0xd0);
  SpiDelay(10);	 
  y=SPI_WriteByte(0x0);
  y<<=8;
  y+=SPI_WriteByte(0x0); 
  T_DCS(); 
  y = y>>4;
y  = y & 0xFFF;	 //fff
  return (y);
}

		   
u8 read_once(void)
{
   Pen_Point.X=TPReadY(); 
   Pen_Point.Y=TPReadX(); 
   return 1;
}	 

 
void NVIC_TOUCHConfiguration(void)
{
  /*
	NVIC_InitTypeDef NVIC_InitStructure;
    
#ifdef  VECT_TAB_RAM  									   
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else   							 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif  
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		 
  //	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQChannel;  //Ê¹ÓÃÍâ²¿ÖÐ¶Ï10~15
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure); 	*/								 
}

void touch_init(void)
{	
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStruct;  
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
 
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOD, ENABLE);
  
  GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Speed=GPIO_Speed_25MHz;
  GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;  

  GPIO_InitStruct.GPIO_Pin=GPIO_Pin_15|GPIO_Pin_13|GPIO_Pin_14;			
  GPIO_Init(GPIOB,&GPIO_InitStruct);			                      
   	  
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);      //sclk	10	 13
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);	//mýso	11	 14
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);	//mosý	12	 15
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);	 
												   
  SPI_I2S_DeInit(SPI2);	    
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; 
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master; 
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; 
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;//SPI_CPOL_Low 	 SPI_CPOL_High
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; 
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;   //SPI_NSS_Hard	 //SPI_NSS_Soft
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; 	//16
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; 
  SPI_InitStructure.SPI_CRCPolynomial = 7; 
  SPI_Init(SPI2,&SPI_InitStructure); 
  SPI_Cmd(SPI2,ENABLE); 
  //CS
  GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;  
  GPIO_InitStruct.GPIO_Pin=GPIO_Pin_12; // 3
  GPIO_Init(GPIOB,&GPIO_InitStruct);    // d  	    			
  T_DCS(); 				 
  //T_PEN				
  GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;  
  GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6;
  GPIO_Init(GPIOD,&GPIO_InitStruct);     
 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource6);  	 
  EXTI_InitStructure.EXTI_Line = EXTI_Line6;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);	    				  
}

  	   
u8 Read_Ads7846(void)
{
	u8 t,t1,count=0;
	u16 databuffer[2][10];
	u16 temp=0;
	while(count<10)
	{	    		   
	   {	if(read_once())
		   {	  
			databuffer[0][count]=Pen_Point.X;
			databuffer[1][count]=Pen_Point.Y;
			count++;  
		    }
		}
	}

	if(count==10)
	{  
	    do
		{	
			t1=0;		  
			for(t=0;t<count-1;t++)
			{
				if(databuffer[0][t]>databuffer[0][t+1])
				{
					temp=databuffer[0][t+1];
					databuffer[0][t+1]=databuffer[0][t];
					databuffer[0][t]=temp;
					t1=1; 
				}  
			}
		}while(t1); 	  
		do
		{	
			t1=0;		 
			for(t=0;t<count-1;t++)
			{
				if(databuffer[1][t]>databuffer[1][t+1])
				{
					temp=databuffer[1][t+1];
					databuffer[1][t+1]=databuffer[1][t];
					databuffer[1][t]=temp;
					t1=1;	 
				}  
			}
		}while(t1);		   
		 	 		  
		Pen_Point.X=(databuffer[0][3]+databuffer[0][4]+databuffer[0][5])/3;
		Pen_Point.Y=(databuffer[1][3]+databuffer[1][4]+databuffer[1][5])/3;	 	 
		xxx=Pen_Point.X;
		yyy=Pen_Point.Y;	
		flag=1;
		return 1;									   
	}
	flag=0;
	return 0;
}	   

void EXTI9_5_IRQHandler(void)		
{
  if(EXTI_GetITStatus(EXTI_Line6) != RESET)
  { 
   // Read_Ads7846();  	  
    EXTI_ClearITPendingBit(EXTI_Line6);
  }  
}
void LCD_WriteRAM_Prepare(void)
{
  LCD_REG = LCD_REG_34;
}
void LCD_WriteRAM(uint16_t RGB_Code)
{
  LCD_RAM = RGB_Code;
}

void Pixel(int16_t x, int16_t y,int16_t c)
{ 
        if (( x > 239 )||( y > 319 )) return;
	LCD_SetCursor(x,y);
	LCD_WriteRAM_Prepare();
	LCD_WriteRAM(c);
}
void PutPixel(int16_t x, int16_t y)
{ 
	if (( x > 239 )||( y > 319 )) return;
	LCD_SetCursor(x, y);
	LCD_WriteRAM_Prepare();
	LCD_WriteRAM(TextColor);
}
void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
  int16_t  D;/* Decision Variable */ 
  uint16_t  CurX;/* Current X Value */
  uint16_t  CurY;/* Current Y Value */ 
  D = 3 - (Radius << 1);
  CurX = 0;
  CurY = Radius;
  while (CurX <= CurY)
  {
    PutPixel(Xpos + CurX, Ypos + CurY);
    PutPixel(Xpos + CurX, Ypos - CurY);
    PutPixel(Xpos - CurX, Ypos + CurY);
    PutPixel(Xpos - CurX, Ypos - CurY);
    PutPixel(Xpos + CurY, Ypos + CurX);
    PutPixel(Xpos + CurY, Ypos - CurX);
    PutPixel(Xpos - CurY, Ypos + CurX);
    PutPixel(Xpos - CurY, Ypos - CurX);
    if (D < 0)
    { 
      D += (CurX << 2) + 6;
    }
    else
    {
      D += ((CurX - CurY) << 2) + 10;
      CurY--;
    }
    CurX++;
  }
}
void LCD_DrawUniLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
  yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0, 
  curpixel = 0;
  
  deltax = ABS(x2 - x1);        /* The difference between the x's */
  deltay = ABS(y2 - y1);        /* The difference between the y's */
  x = x1;                       /* Start x off at the first pixel */
  y = y1;                       /* Start y off at the first pixel */
  
  if (x2 >= x1)                 /* The x-values are increasing */
  {
    xinc1 = 1;
    xinc2 = 1;
  }
  else                          /* The x-values are decreasing */
  {
    xinc1 = -1;
    xinc2 = -1;
  }
  
  if (y2 >= y1)                 /* The y-values are increasing */
  {
    yinc1 = 1;
    yinc2 = 1;
  }
  else                          /* The y-values are decreasing */
  {
    yinc1 = -1;
    yinc2 = -1;
  }
  
  if (deltax >= deltay)         /* There is at least one x-value for every y-value */
  {
    xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
    yinc2 = 0;                  /* Don't change the y for every iteration */
    den = deltax;
    num = deltax / 2;
    numadd = deltay;
    numpixels = deltax;         /* There are more x-values than y-values */
  }
  else                          /* There is at least one y-value for every x-value */
  {
    xinc2 = 0;                  /* Don't change the x for every iteration */
    yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
    den = deltay;
    num = deltay / 2;
    numadd = deltax;
    numpixels = deltay;         /* There are more y-values than x-values */
  }
  
  for (curpixel = 0; curpixel <= numpixels; curpixel++)
  {
    PutPixel(x, y);             /* Draw the current pixel */
    num += numadd;              /* Increase the numerator by the top of the fraction */
    if (num >= den)             /* Check if numerator >= denominator */
    {
      num -= den;               /* Calculate the new numerator value */
      x += xinc1;               /* Change the x as appropriate */
      y += yinc1;               /* Change the y as appropriate */
    }
    x += xinc2;                 /* Change the x as appropriate */
    y += yinc2;                 /* Change the y as appropriate */
  }
}


void Drow_Touch_Point(uint16_t x,uint16_t y)
{
	uint16_t TextColor = 0x0000;(x-12,y,x+13,y);
	LCD_DrawUniLine(x,y-12,x,y+13);
	Pixel(x+1,y+1,Blue);
	Pixel(x-1,y+1,Blue);
	Pixel(x+1,y-1,Blue);
	Pixel(x-1,y-1,Blue);
	LCD_DrawCircle(x,y,6);
}	  
void Draw_Big_Point(uint16_t x,uint16_t y)
{	    
	Pixel(x,y,Blue);
	Pixel(x+1,y,Blue);
	Pixel(x,y+1,Blue);
	Pixel(x+1,y+1,Blue);
}
void Convert_Pos(void)
{		
  Read_Ads7846();
  Pen_Point.X0=Pen_Point.xfac*Pen_Point.X+Pen_Point.xoff;
  Pen_Point.Y0=Pen_Point.yfac*Pen_Point.Y+Pen_Point.yoff;  
	
}

void Touch_Adjust(void)
{								 
	uint16_t pos_temp[4][2];
	uint8_t  cnt=0;	
	uint16_t d1,d2;
	uint32_t tem1,tem2;
	float fac; 	   
	cnt=0;				
	LCD_SetTextColor(Blue);
	LCD_SetTextColor(White);
	LCD_Clear(White);
	Drow_Touch_Point(20,20);
	Pen_Point.xfac=0;
	while(1)
	{
                if(1==1)
		//if(Pen_Point.Key_Sta==Key_Down)
		{
			if(Read_Ads7846())
			{  								   
				pos_temp[cnt][0]=Pen_Point.X;
				pos_temp[cnt][1]=Pen_Point.Y;
				cnt++;
                                Delay(0xFFFF);
			}			 
			switch(cnt)
			{			   
				case 1:
					LCD_Clear(White);
					Drow_Touch_Point(210,20);
					break;
				case 2:
					LCD_Clear(White);
					Drow_Touch_Point(20,300);
					break;
				case 3:
					LCD_Clear(White);
					Drow_Touch_Point(210,300);
					break;
				case 4:
					tem1=abs(pos_temp[0][0]-pos_temp[1][0]);
					tem2=abs(pos_temp[0][1]-pos_temp[1][1]);
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);
					
					tem1=abs(pos_temp[2][0]-pos_temp[3][0]);
					tem2=abs(pos_temp[2][1]-pos_temp[3][1]);
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);
					fac=(float)d1/d2;
					if(fac<0.75||fac>1.25||d1==0||d2==0)
					{
						cnt=0;
						LCD_Clear(White);
						Drow_Touch_Point(20,20);
						continue;
					}
					tem1=abs(pos_temp[0][0]-pos_temp[2][0]);
					tem2=abs(pos_temp[0][1]-pos_temp[2][1]);
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);
					
					tem1=abs(pos_temp[1][0]-pos_temp[3][0]);
					tem2=abs(pos_temp[1][1]-pos_temp[3][1]);
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);
					fac=(float)d1/d2;
					if(fac<0.75||fac>1.25)
					{
						cnt=0;
						LCD_Clear(White);
						Drow_Touch_Point(20,20);
						continue;
					}
					tem1=abs(pos_temp[1][0]-pos_temp[2][0]);
					tem2=abs(pos_temp[1][1]-pos_temp[2][1]);
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);
	
					tem1=abs(pos_temp[0][0]-pos_temp[3][0]);
					tem2=abs(pos_temp[0][1]-pos_temp[3][1]);
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);
					fac=(float)d1/d2;
					if(fac<0.75||fac>1.25)
					{
						cnt=0;
						LCD_Clear(White);
						Drow_Touch_Point(20,20);
						continue;
					}
					Pen_Point.xfac=(float)200/(pos_temp[1][0]-pos_temp[0][0]);
					Pen_Point.xoff=(240-Pen_Point.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;

					Pen_Point.yfac=(float)280/(pos_temp[2][1]-pos_temp[0][1]);
					Pen_Point.yoff=(320-Pen_Point.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;
					LCD_Clear(White);
					return ;


			}
		}
	}
}
