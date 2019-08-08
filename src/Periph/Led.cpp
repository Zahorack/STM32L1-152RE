/*
 * Led.cpp
 *
 *  Created on: Jun 18, 2018
 *      Author: xgallom
 */

#include "Periph/Led.h"

namespace Periph {

Led::Led(Leds::Enum id) :
		id(id)
{

	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef init = {
		.Pin = id,
		.Mode = GPIO_MODE_OUTPUT_PP,
		.Pull = GPIO_NOPULL,
		.Speed = GPIO_SPEED_MEDIUM,
	};

	HAL_GPIO_Init(GPIOA, &init);
}

Led::~Led() {
	turnOff();
}


void Led::turnOn()
{
	HAL_GPIO_WritePin(GPIOA, id, GPIO_PIN_SET);
}

void Led::turnOff()
{
	HAL_GPIO_WritePin(GPIOA, id, GPIO_PIN_RESET);
}

void Led::toggle()
{
	HAL_GPIO_TogglePin(GPIOA, id);
}

} /* namespace periph */
