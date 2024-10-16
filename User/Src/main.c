/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * Description of project
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ts.h"
#include "ts_calibration.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define Max 4
/* Private variables ---------------------------------------------------------*/
int switchTimer = 0;
int switchColour = 0;

int cnt[2]={0,0};




/* Private function prototypes -----------------------------------------------*/
static int GetUserButtonPressed(void);
static int GetTouchState (int *xCoord, int *yCoord);

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{
	HAL_IncTick();
	cnt[switchTimer]++;
}

void EXTI0_IRQHandler(void) {
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
	switchTimer=!switchTimer;




}
void EXTI3_IRQHandler(void) {

	switchColour++;
	if(switchColour==Max){
		switchColour=0;
	}


}


/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* MCU Configuration--------------------------------------------------------*/
	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();
	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize LCD and touch screen */
	LCD_Init();
	TS_Init(LCD_GetXSize(), LCD_GetYSize());
	/* touch screen calibration */
	//	TS_Calibration();

	/* Clear the LCD and display basic starter text */
	LCD_Clear(LCD_COLOR_BLACK);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BLACK);
	LCD_SetFont(&Font20);
	// There are 2 ways to print text to screen: using printf or LCD_* functions
	LCD_DisplayStringAtLine(0, "    HTL Wels");
	// printf Alternative
	LCD_SetPrintPosition(1, 0);
	printf(" Fischergasse 30");
	LCD_SetPrintPosition(2, 0);
	printf("   A-4600 Wels");

	LCD_SetFont(&Font8);
	LCD_SetColors(LCD_COLOR_MAGENTA, LCD_COLOR_BLACK); // TextColor, BackColor
	LCD_DisplayStringAtLineMode(39, "copyright xyz", CENTER_MODE);


	GPIO_InitTypeDef Switch_PA0;

	Switch_PA0.Alternate = 0;
	Switch_PA0.Mode = GPIO_MODE_IT_RISING;
	Switch_PA0.Pin = GPIO_PIN_0;
	Switch_PA0.Pull = GPIO_NOPULL;
	Switch_PA0.Speed = GPIO_SPEED_FAST;

	HAL_GPIO_Init(GPIOA, &Switch_PA0);

	HAL_NVIC_EnableIRQ(EXTI0_IRQn);

	GPIO_InitTypeDef Switch_PG3;

	Switch_PG3.Alternate = 0;
	Switch_PG3.Mode = GPIO_MODE_IT_RISING;
	Switch_PG3.Pin = GPIO_PIN_3;
	Switch_PG3.Pull = GPIO_NOPULL;
	Switch_PG3.Speed = GPIO_SPEED_FAST;

	HAL_GPIO_Init(GPIOG, &Switch_PG3);

	HAL_NVIC_EnableIRQ(EXTI3_IRQn);










	int Colour[Max]={LCD_COLOR_BLUE, LCD_COLOR_RED, LCD_COLOR_GREEN, LCD_COLOR_YELLOW};
	/* Infinite loop */
	while (1)
	{





		// display timer


		LCD_SetFont(&Font20);
		LCD_SetTextColor(Colour[switchColour]);
		LCD_SetPrintPosition(5, 0);
		printf("   Timer: %.2f", cnt[0]/1000.0);

		// display timer 2


		LCD_SetFont(&Font20);
		LCD_SetTextColor(Colour[switchColour]);
		LCD_SetPrintPosition(7, 0);
		printf("   Timer: %.2f", cnt[1]/1000.0);




		// test touch interface
		int x, y;
		if (GetTouchState(&x, &y)) {
			LCD_FillCircle(x, y, 5);
		}


	}
}

/**
 * Check if User Button has been pressed
 * @param none
 * @return 1 if user button input (PA0) is high
 */
static int GetUserButtonPressed(void) {
	return (GPIOA->IDR & 0x0001);
}

/**
 * Check if touch interface has been used
 * @param xCoord x coordinate of touch event in pixels
 * @param yCoord y coordinate of touch event in pixels
 * @return 1 if touch event has been detected
 */
static int GetTouchState (int* xCoord, int* yCoord) {
	void    BSP_TS_GetState(TS_StateTypeDef *TsState);
	TS_StateTypeDef TsState;
	int touchclick = 0;

	TS_GetState(&TsState);
	if (TsState.TouchDetected) {
		*xCoord = TsState.X;
		*yCoord = TsState.Y;
		touchclick = 1;
		if (TS_IsCalibrationDone()) {
			*xCoord = TS_Calibration_GetX(*xCoord);
			*yCoord = TS_Calibration_GetY(*yCoord);
		}
	}

	return touchclick;
}


