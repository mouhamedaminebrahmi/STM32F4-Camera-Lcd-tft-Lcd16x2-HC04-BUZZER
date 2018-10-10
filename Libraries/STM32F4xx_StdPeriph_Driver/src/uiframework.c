/*******************************************************************************
* @file		: uiframework.c
* @author	: IMS Systems LAB & Technical Marketing
* @version	: V1.0.0
* @date		: 20-Nov-2017
* @brief	: Screen Code
*******************************************************************************/
/* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* <h2><center>&copy; COPYRIGHT 2017 STMicroelectronics</center></h2>
*******************************************************************************
*/ 
/*includes----------------------------------------------------------------------*/
#include "uiframework.h"
#include "uiappuser.h"
#include "float.h"
#include "LcdHal.h"
#include "TscHal.h"
#include "pictures.h"

GL_Page_TypeDef *CurrentScreen;

/** @addtogroup Embedded_GUI_Example
  * @{
  */

/** @defgroup UI_Framework
  * @brief This file contains functions to configure the screens and controls
  * @{
  */

/* External variables ----------------------------------------------------------*/
/* Private typedef -------------------------------------------------------------*/
/* Private defines -------------------------------------------------------------*/
/* Private macros --------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------*/
GL_Page_TypeDef welcome;
/* Private function prototypes -------------------------------------------------*/
/* Private functions -----------------------------------------------------------*/
/**
  * @brief Create and configure screen object
  * @note This control creates the screen object and configures contained controls
  * @param None
  * @retval None
  */
void Create_welcome() 
{ 

	GL_PageControls_TypeDef* DesignIcon02= NewIcon (1,DesignIcon02_Image,320,240,welcome_DesignIcon02_Click);
	Create_PageObj( &welcome ); 
	AddPageControlObj(319,0,DesignIcon02,&welcome);
}




/**
  * @brief Show screen object
  * @note This control displayed the specified screen object
  * @param None
  * @retval None
  */
void Show_welcome()
{ 
	if(CurrentScreen!=GL_NULL)
	{
		CurrentScreen->ShowPage(CurrentScreen,GL_FALSE);
	}
	GL_Clear(65535);
	GL_SetTextColor(63488);
	welcome.ShowPage(&welcome, GL_TRUE);
	CurrentScreen=&welcome; 
} 

/**
  * @brief Controls home screen
  * @param None
  * @retval None
  */
void Show_HomeScreen()
{
	Create_welcome();
	CurrentScreen = &welcome;
	GL_Clear(65535);
	GL_SetTextColor(63488);
	welcome.ShowPage(&welcome,GL_TRUE);
}

/**
  * @}
  */

/**
  * @}
  */


/******************* (C) COPYRIGHT 2017 STMicroelectronics *****END OF FILE****/ 

