/*
 * SysTick.cpp
 *
 *  Created on: Jun 25, 2018
 *      Author: xgallom
 */

#include <Periph/SysTickCounter.h>
#include "Util/Trace.h"

namespace Periph {

static volatile micros_t s_millisSinceLastReset;
static bool s_wasInitialized = false;

void SysTickCounter::init()
{
	TRACE("Systick init start\n\r");

	if(SysTick_Config((uint32_t)(HAL_RCC_GetHCLKFreq() / 1000)) == 0) {
		TRACE("Systick init success\n\r");
	}
	else {
		TRACE("Systick init error\n\r");
	}
}

SysTickCounter::SysTickCounter()
{
	if(!s_wasInitialized) {
		init();
		s_millisSinceLastReset = 0;

		s_wasInitialized = true;
	}
}

micros_t SysTickCounter::millisSinceLastReset() const
{
	return s_millisSinceLastReset;
}

Util::Time SysTickCounter::sinceLastReset() const
{
	return Util::Time::FromMilliSeconds(millisSinceLastReset());
}

} /* namespace Periph */

void SysTick_Handler(void)
{
	++Periph::s_millisSinceLastReset;
	//TRACE("%u\n\r", Periph::s_millisSinceLastReset);
}
