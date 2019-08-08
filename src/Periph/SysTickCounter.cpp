/*
 * SysTick.cpp
 *
 *  Created on: Jun 25, 2018
 *      Author: xgallom
 */

#include <Periph/SysTickCounter.h>
#include "Util/Trace.h"

namespace Periph {

static volatile micros_t s_microsSinceLastReset;
static bool s_wasInitialized = false;

void SysTickCounter::init()
{
	uint32_t cfg = (uint32_t)(SystemCoreClock / 100000);
	while(SysTick_Config(cfg)!= 0)
	{}
}

SysTickCounter::SysTickCounter()
{
	if(!s_wasInitialized) {
		init();
		s_microsSinceLastReset = 0;

		s_wasInitialized = true;
	}
}

micros_t SysTickCounter::microsSinceLastReset() const
{
	return s_microsSinceLastReset;
}

Util::Time SysTickCounter::sinceLastReset() const
{
	return Util::Time::FromMicroSeconds(microsSinceLastReset());
}

} /* namespace Periph */

void SysTick_Handler(void)
{
	//TRACE("a");
	++Periph::s_microsSinceLastReset;
}
