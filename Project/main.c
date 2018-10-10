#include "stm32f4xx.h"
#include "misc.h"
#include "LCD.h"
#include "pictures.h"
#include "dcmi_OV7670.h"
#include <stdio.h>



void Delay(__IO uint32_t nCount);
GPIO_InitTypeDef gpio; //TypeDef est une structure 
EXTI_InitTypeDef Exti;
NVIC_InitTypeDef Nvic;



uint32_t distance=0;


__IO uint32_t time = 0;

GPIO_InitTypeDef GPIO_struct_hc04;
OV7670_IDTypeDef OV7670ID;


GPIO_InitTypeDef gpio;
GPIO_InitTypeDef  GPIO_InitStructure;
GPIO_InitTypeDef PinInitStruct;
GPIO_InitTypeDef GPIO_InitDef;
void startCounter();
uint32_t getCounterValue();
void waitUs( uint32_t nCount);


void configureHc04();
void readHc04();


void EXTI0_IRQHandler(void) { // si deux interruption sur la meme ligne PA1 et PC0 en fait une autre void sinon sur la meme void exp PA0 etPB0 
    /* Make sure that interrupt flag is set */
    if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
      LCD_Initializtion();
LCD_Clear(Blue2);
GUI_Text(100,100,"Mode Marche Arriere",Blue2,White);

Delay(0xFFFFFF);
LCD_Clear(White);

OV7670_IDTypeDef OV7670ID;
DCMI_OV7670_Init();
DCMI_OV7670_ReadID(&OV7670ID);
DMA_Cmd(DMA2_Stream1, ENABLE);//Activation du Dma Pour transmmettre les données Du DCMI(Caméra) au FSMC(Ecran Lcd)
DCMI_Cmd(ENABLE);

DCMI_CaptureCmd(ENABLE); //Activation du camera   
     
    /* Clear interrupt flag */
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}




int main(void)
{
  
LCD_Initializtion();
LCD_Clear(0XFFFFF);       
DrawPicture(0, 0, 320, 240, (uint16_t*) *DesignIcon01_Image);


  configureHc04();

 SysTick_Config(SystemCoreClock/1000000);
 
 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
gpio.GPIO_Pin = GPIO_Pin_0;
  gpio.GPIO_Mode = GPIO_Mode_IN;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB,&gpio);
   SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
   /* PA0... PD0 is connected to EXTI_Line0 */
   Exti.EXTI_Line = EXTI_Line0;
    /* Interrupt mode */
    Exti.EXTI_Mode = EXTI_Mode_Interrupt;
     /* Triggers on rising and falling edge */
    Exti.EXTI_Trigger = EXTI_Trigger_Falling;//rising en passent à l'etat1 et filing en passant à l'etat 0
    /* Enable interrupt */
    Exti.EXTI_LineCmd = ENABLE;
    /* Add to EXTI */
    EXTI_Init(&Exti);
    
    
   Nvic.NVIC_IRQChannel = EXTI0_IRQn;
    /* Set priority */
   Nvic.NVIC_IRQChannelPreemptionPriority = 0x00;//permet de configurer le priorité de l'interruption se met quand il'ya plus qu'une seule interruption 
     /* Set sub priority */
    Nvic.NVIC_IRQChannelSubPriority = 0x00;//permet de configurer le priorité quand les deux interruptions sont dans la meme ligne 
    /* Enable interrupt */
    Nvic.NVIC_IRQChannelCmd = ENABLE;
    /* Add to NVIC */
    NVIC_Init(&Nvic);
 GPIO_InitDef.GPIO_Pin =GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT; 
GPIO_InitDef.GPIO_OType = GPIO_OType_PP; 
GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_DOWN; 
GPIO_InitDef.GPIO_Speed = GPIO_Speed_50MHz; 
GPIO_Init(GPIOA, &GPIO_InitDef);


PinInitStruct.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_0|GPIO_Pin_3|GPIO_Pin_1; 
PinInitStruct.GPIO_Mode = GPIO_Mode_OUT; 
PinInitStruct.GPIO_OType = GPIO_OType_PP; 
PinInitStruct.GPIO_PuPd = GPIO_PuPd_DOWN; 
PinInitStruct.GPIO_Speed = GPIO_Speed_50MHz; 
GPIO_Init(GPIOC, &PinInitStruct);
        

  
while (1)
{ 
readHc04();

   GPIO_SetBits(GPIOA, GPIO_Pin_3);
GPIO_SetBits(GPIOA, GPIO_Pin_1);
GPIO_ResetBits(GPIOA, GPIO_Pin_2);
Delay(0x1FFFFF);
GPIO_SetBits(GPIOA, GPIO_Pin_2);
GPIO_ResetBits(GPIOA, GPIO_Pin_1);
Delay(0x1FFFFF);
           if (distance>500)
{
	
        GPIO_ResetBits(GPIOC,GPIO_Pin_5);
        GPIO_ResetBits(GPIOC,GPIO_Pin_0);
        GPIO_ResetBits(GPIOC,GPIO_Pin_1);
        GPIO_SetBits(GPIOC,GPIO_Pin_3);
        
}

        if (distance<499)
        {
GPIO_SetBits(GPIOC,GPIO_Pin_1);
GPIO_ResetBits(GPIOC,GPIO_Pin_0);
GPIO_ResetBits(GPIOC,GPIO_Pin_3);
GPIO_ResetBits(GPIOC,GPIO_Pin_5);
        }

    if(distance<100)
           {
  
        GPIO_SetBits(GPIOC,GPIO_Pin_5);
        GPIO_SetBits(GPIOC,GPIO_Pin_0);
        GPIO_ResetBits(GPIOC,GPIO_Pin_3);
        GPIO_ResetBits(GPIOC,GPIO_Pin_1);


}

}
}
void waitUs( uint32_t nCount)
{
  startCounter();
  while (getCounterValue() < nCount); 

}

void startCounter()
{
  time = 1;
}

uint32_t getCounterValue()
{

  return time;

}

void configureHc04()
{

 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
 
 GPIO_struct_hc04.GPIO_Pin = GPIO_Pin_12;
 GPIO_struct_hc04.GPIO_Mode = GPIO_Mode_OUT;
 GPIO_struct_hc04.GPIO_Speed = GPIO_Speed_2MHz;
 GPIO_struct_hc04.GPIO_OType = GPIO_OType_PP;
 
 GPIO_Init(GPIOB,&GPIO_struct_hc04);
 
 
 GPIO_struct_hc04.GPIO_Pin = GPIO_Pin_13;
 GPIO_struct_hc04.GPIO_Mode = GPIO_Mode_IN;
 GPIO_struct_hc04.GPIO_PuPd = GPIO_PuPd_NOPULL;
 
 GPIO_Init(GPIOB,&GPIO_struct_hc04);
}
void readHc04()
{
  
uint32_t highPulseWidth;

GPIO_SetBits(GPIOB, GPIO_Pin_12);
waitUs(10);
GPIO_ResetBits(GPIOB, GPIO_Pin_12);

while( GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13) == 0);
startCounter();

while( GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13) == 1);
highPulseWidth = getCounterValue();

distance = ((highPulseWidth*340)/2000);


}




//Declaration de la Fonction du Delay************************************************************************
void Delay(__IO uint32_t nCount)
{
  while(nCount--)
  {
  }
}

  
  
 
