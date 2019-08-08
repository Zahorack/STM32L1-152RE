/*
 * Led.h
 *
 *  Created on: Jun 18, 2018
 *      Author: xgallom
 */

#ifndef PERIPH_LED_H_
#define PERIPH_LED_H_

#include "stm32l1xx.h"

namespace Periph {

namespace Leds {
enum Enum : uint16_t {
	Green = GPIO_PIN_5,
};
}

class Led {
	const Leds::Enum id;

public:
	Led(Leds::Enum id);
	~Led();

	void turnOn();
	void turnOff();
	void toggle();
};

} /* namespace Periph */

#endif /* PERIPH_LED_H_ */
