/*
 * Ultrasonic.cpp
 *
 *  Created on: 27.9.2079
 *      Author: Zahorack
 */

#include "Periph/Ultrasonic.h"
#include "Util/Trace.h"

namespace Periph {

static const struct {
	GPIO_TypeDef *port;
	uint16_t pin;
	IRQn_Type Irs;
} config[Ultrasonics::Size]= {
	{/* Ultrasonic1 */
			port: GPIOA,
			pin: GPIO_PIN_8,
			Irs: EXTI9_5_IRQn,
	},
	{/* Ultrasonic2 */
			port: GPIOB,
			pin: GPIO_PIN_10,
			Irs: EXTI15_10_IRQn,
	}
};

Ultrasonic::Ultrasonic(Ultrasonics::Enum id):
	m_id(id)
{
	configure(PinFunction::Echo);
}



void Ultrasonic::configure(PinFunction::Enum fcn)
{
	if(config[m_id].port == GPIOA)
		__GPIOA_CLK_ENABLE();
	else if(config[m_id].port == GPIOB)
		__GPIOB_CLK_ENABLE();
	else if(config[m_id].port == GPIOC)
		__GPIOC_CLK_ENABLE();
	else if(config[m_id].port == GPIOD)
		__GPIOD_CLK_ENABLE();
	else if(config[m_id].port == GPIOE)
		__GPIOE_CLK_ENABLE();
	else if(config[m_id].port == GPIOF)
		__GPIOF_CLK_ENABLE();
	else if(config[m_id].port == GPIOG)
		__GPIOG_CLK_ENABLE();
	else if(config[m_id].port == GPIOH)
		__GPIOH_CLK_ENABLE();

	GPIO_InitTypeDef init;
	init.Pin = 	static_cast<uint32_t>(config[m_id].pin);

	switch(fcn) {
		case PinFunction::Echo :
			TRACE("Echo");
			init.Mode = 	GPIO_MODE_IT_RISING_FALLING;
			init.Pull = 	GPIO_PULLUP;
			HAL_NVIC_SetPriority(config[m_id].Irs, 3, 0);
			HAL_NVIC_EnableIRQ(config[m_id].Irs);
			break;

		case PinFunction::Trigger :
			TRACE("Trigger");
			init.Mode = 	GPIO_MODE_OUTPUT_OD;
			init.Pull = 	GPIO_PULLUP;
			HAL_NVIC_DisableIRQ(config[m_id].Irs);
			break;
	}



	init.Speed = 	GPIO_SPEED_HIGH;

	HAL_GPIO_Init(config[m_id].port, &init);
	TRACE("Ultrasonic inti ok \n\r");
}


extern "C" void EXTI9_5_IRQHandler(void)
{
	TRACE("EXTI9_5_IRQHandler\n\r");
	if(__HAL_GPIO_EXTI_GET_FLAG(GPIO_PIN_8)){
		TRACE("EXTI9_5_IRQHandler Pin8\r\n");
	}
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
}

extern "C" void EXTI15_10_IRQHandler(void)
{
	if(__HAL_GPIO_EXTI_GET_FLAG(GPIO_PIN_15)){

	}
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
}




}