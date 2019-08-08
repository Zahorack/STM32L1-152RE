/*
 * Dir.cpp
 *
 *  Created on: Jun 22, 2018
 *      Author: Zahorack
 */

#include <Periph/DigitalOutputPin.h>

namespace Periph {

void DigitalOutputPin::initGpio()
{
	if(port == GPIOA)
		__GPIOA_CLK_ENABLE();
	else if(port == GPIOB)
		__GPIOB_CLK_ENABLE();
	else if(port == GPIOC)
		__GPIOC_CLK_ENABLE();
	else if(port == GPIOD)
		__GPIOD_CLK_ENABLE();
	else if(port == GPIOE)
		__GPIOE_CLK_ENABLE();
	else if(port == GPIOF)
		__GPIOF_CLK_ENABLE();
	else if(port == GPIOG)
		__GPIOG_CLK_ENABLE();
	else if(port == GPIOH)
		__GPIOH_CLK_ENABLE();



	GPIO_InitTypeDef init = {
		.Pin = 	static_cast<uint32_t>(id),
		.Mode = 	GPIO_MODE_OUTPUT_PP,
		.Pull = 	GPIO_NOPULL,
		.Speed = 	GPIO_SPEED_MEDIUM,
	};

	HAL_GPIO_Init(port, &init);
}

DigitalOutputPin::DigitalOutputPin(GPIO_TypeDef *a_port, uint16_t a_id):
	id(a_id),
	port(a_port)
{
	initGpio();
}

DigitalOutputPin::~DigitalOutputPin()
{}

void DigitalOutputPin::setPin()
{
	HAL_GPIO_WritePin(port, id, GPIO_PIN_SET);
}

void DigitalOutputPin::resetPin()
{
	HAL_GPIO_WritePin(port, id, GPIO_PIN_RESET);
}

void DigitalOutputPin::setPinTo(bool on)
{
	if(on)
		setPin();
	else
		resetPin();
}

void DigitalOutputPin::togglePin()
{
	HAL_GPIO_TogglePin(port, id);
}

bool DigitalOutputPin::readPin() const
{
	return HAL_GPIO_ReadPin(port, id) ? true : false;
}

void DigitalOutputPin::deinitGpio()
{
}

} /* namespace Periph */
