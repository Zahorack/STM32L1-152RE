/*
 * Buzzer.cpp
 *
 *  Created on: 24.8.2019
 *      Author: Zahorack
 */


#include "Util/Trace.h"
#include "Periph/Buzzer.h"

namespace Periph {


static const struct {
	GPIO_TypeDef *gpio;
	uint16_t pin;
} config[Buzzers::Size] = {
 	/* Buzzer 1 */
	{
		gpio: GPIOB,
		pin: GPIO_PIN_8
	}
};

Buzzer::Buzzer(Buzzers::Enum id):
	m_id(id),
	m_state(false),
	m_pin(config[id].gpio, config[id].pin),
	m_timer(Util::Time::FromMilliSeconds(500))
{
	m_timer.start();
}

void Buzzer::start() {
	m_state = true;
}

void Buzzer::stop() {
	m_state = false;
	m_pin.resetPin();
}

void Buzzer::update() {
	if(m_timer.run() && m_state) {
		m_pin.togglePin();
	}
}

}