#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "charlcd.h"

u8 CharLCD_line = 1;
u8 CharLCD_column = 1;

// Configures the gpio pins and clocks on the microcontroller
void CharLCD_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// Open the clocks we want
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB,ENABLE);

	// Configure the LCD pins for push-pull output
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	// Control pins
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
}

// Initializes the LCD itself
void CharLCD_Init(void)
{
	CharLCD_WriteData(0x00);
	Set_RS;
	Set_RW;
	Set_En;

	CharLCD_Delay(0xFFFF);

	Clr_RS;
	Clr_RW;
	Clr_En;

	// 8-bit, 2-line (4-line displays use 2-lines with more characters per line)
	if(Num_Lines > 1)
	{
		CharLCD_WriteData(0x38);
	}else if(Num_Lines == 1)
	{
		CharLCD_WriteData(0x30);
	}
	// Turn display on, with cursor on and blinking
	CharLCD_WriteData(0x0F);

	// increment left, no screen shift
	CharLCD_WriteData(0x06);

	CharLCD_Delay(0xFF);

	CharLCD_Clear();
}

void CharLCD_WriteLineWrap(const char* string) 
{
	char line[(Num_Characters + 1)];

	u8 i,j,k,l;
	k = 0;
	l = 0;
	for(j = CharLCD_line;j <= Num_Lines;j++){
		for(i = CharLCD_column;i <= Num_Characters;i++){
			line[l] = string[k];
			if(line[l] == '\0') {
				CharLCD_WriteString(line);
				return;
			}
			k++;
			l++;
		}
		line[l] = '\0';
		CharLCD_WriteString(line);
		CharLCD_SetCursor(j+1,1);
		l = 0;
	}
}


void CharLCD_WriteLineNoWrap(const char* string) 
{
	char line[(Num_Characters + 1)];

	u8 i,j;
	j = 0;
	for(i = CharLCD_column;i <= Num_Characters;i++){
		line[j] = string[j];
		j++;
	}
	line[j] = '\0';

	CharLCD_WriteString(line);
}


void CharLCD_WriteString(const char* line) 
{
	int i;
	Set_RS;
	for(i = 0;i < 80;i++){
		if(line[i] == '\0' || !line[i]){
			return;
		}else {
			CharLCD_WriteData((int)line[i]);
		}
	}
	Clr_RS;
}

void CharLCD_SendCustom(CustomCharacter *character)
{
	if(character->number <= 7){
		Clr_RS;
		Clr_RW;

		u8 templine = CharLCD_line;
		u8 tempcolumn = CharLCD_column;

		CharLCD_WriteData(0x40 | (character->number));

		Set_RS;
		u8 i;
		for(i = 0;i < 8;i++){
			CharLCD_WriteData(character->line[i]);
		}
		Clr_RS;

		CharLCD_SetCursor(templine,tempcolumn);
	} // else: learn more about the LCD you're using :)
}

/*
 * Writes a custom character to the screen
 */
void CharLCD_WriteCustom(CustomCharacter *character)
{
	Clr_RW;

	Set_RS;
	CharLCD_WriteData(character->number);
	Clr_RS;
}

void CharLCD_SetCursor(u8 line,u8 column)
{
	Clr_RS;
	Clr_RW;
	u8 position;
	
	switch(line) {
		case 1:
			position = column - 0x01;
			break;
		case 2:
			position = column + 0x3F;
			break;
		case 3:
			position = column + 0x13;
			break;
		case 4:
			position = column + 0x53;
			break;
		default:
			break;
	}

	CharLCD_WriteData(position | 0x80);

	CharLCD_line = line;
	CharLCD_column = column;
}

void CharLCD_Clear(void)
{
	Clr_RS;
	Clr_RW;
	CharLCD_WriteData(0x01);

	CharLCD_line = 1;
	CharLCD_column = 1;
	CharLCD_Delay(0xFFF);
}

void CharLCD_Delay(int Count)
{
	while(Count--)
	{
	}
}

void CharLCD_Test(void)
{
	int i;
	Set_RS;
	for(i = 33;i < 33+(Num_Characters*Num_Lines);i++){
		CharLCD_WriteData(i);
	}
}

/*
 * Host write timing diagram:
 *     ------\ /-----------------\ /------
 * RS (low if X writing register, X high if not)
 *     ______/ \_________________/ \______
 *
 *     ------\                    /-------
 * R/W        \                  /
 *             \________________/
 *
 *             /-------\          /-------
 * E          /  175ns  \        /
 *     ______/           \______/
 *
 *                   /-------\
 * DB0-DB7          /         \
 *     ____________/           \__________
 *
 */
void CharLCD_WriteData(u8 data)
{
	GPIOE->ODR=((GPIOE->ODR & 0xF00F) | (data << 4));

	Set_En;

	CharLCD_Delay(0xFF);

	Clr_En;

	CharLCD_Delay(0xFF);

	GPIOE->ODR=((GPIOE->ODR & 0xF00F));

	CharLCD_IncrementCursorVariables();
}

void CharLCD_Backlight(u8 status)
{
	if(status)
	{
		Backlight_On;
	}else
	{
		Backlight_Off;
	}
}

void CharLCD_IncrementCursorVariables(void)
{
	if((CharLCD_column + 1) <= Num_Characters) {
		CharLCD_column++;
	}else if((CharLCD_line + 1) <= Num_Lines) {
		CharLCD_line++;
		CharLCD_column = 1;
	}else {
		CharLCD_line = 1;
		CharLCD_column = 1;
	}
}

