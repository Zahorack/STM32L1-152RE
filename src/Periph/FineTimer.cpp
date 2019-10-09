/*
 * FineTimer.cpp
 *
 *  Created on: 27.9.2079
 *      Author: Zahorack
 */

#include "Periph/FineTimer.h"
#include "Util/Trace.h"

namespace Periph {

static TIM_HandleTypeDef TIM_Handle;
static uint64_t s_periods = 0;
static const uint16_t TimerPeriod = 0xFFFF;

static void init(){
	__TIM7_CLK_ENABLE();   //16MHz
	TRACE("PCLK7  %d\n\r",HAL_RCC_GetPCLK1Freq());

	TIM_Handle.Init.Prescaler = 32;
	TIM_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
	TIM_Handle.Init.Period = 0xFFFF;
	TIM_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	TIM_Handle.Instance = TIM7;

	HAL_TIM_Base_Init(&TIM_Handle);
	HAL_TIM_Base_Start_IT(&TIM_Handle);

	HAL_NVIC_SetPriority(TIM7_IRQn,0, 0);
	HAL_NVIC_EnableIRQ(TIM7_IRQn);

	TRACE("TIMER 7 INIT OK\n\r");
}

FineTimer::FineTimer()
{
	init();
}

uint64_t FineTimer::read()
{
	return s_periods*(0xFFFF) + __HAL_TIM_GET_COUNTER(&TIM_Handle);;
}

void FineTimer::update()
{
//    TRACE("%d\n\r",__HAL_TIM_GET_COUNTER(&TIM_Handle));
}


extern "C" void TIM7_IRQHandler(void)
{
	if (__HAL_TIM_GET_FLAG(&TIM_Handle, TIM_FLAG_UPDATE) != RESET){     //In case other interrupts are also running
		if (__HAL_TIM_GET_ITSTATUS(&TIM_Handle, TIM_IT_UPDATE) != RESET){
			__HAL_TIM_CLEAR_FLAG(&TIM_Handle, TIM_FLAG_UPDATE);
			/*put your code here */
			//TRACE("TIM7_IRQHandler\n\r");
            s_periods++;
		}
	}
}

}