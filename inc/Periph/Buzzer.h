/*
 * Buzzer.h
 *
 *  Created on: 24.8.2019
 *      Author: Zahorack
 */

#ifndef BUZZER_H_
#define BUZZER_H_

#include "stm32l1xx.h"
#include "Periph/DigitalOutputPin.h"
#include "Util/Timer.h"

namespace Periph {


namespace Buzzers {
enum Enum : uint8_t {
	Buzzer1 = 0,

	Size
};
}

class Buzzer {
	Buzzers::Enum m_id;
	bool m_state;

	Periph::DigitalOutputPin m_pin;
	Util::Timer m_timer;

public:
	Buzzer(Buzzers::Enum id);

	void update();

	void start();
	void stop();
};
}

#endif