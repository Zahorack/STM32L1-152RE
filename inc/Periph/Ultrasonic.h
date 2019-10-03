/*
 *  Ultrasonic.h
 *
 *  Created on: 24.9.2019
 *      Author: Zahorack
 */

#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

#include "stm32l1xx.h"
#include "Util/Timer.h"
#include "Periph/SysTickCounter.h"
#include "Container/Result.h"

namespace Periph {

namespace Ultrasonics {
enum Enum : uint8_t {
	Ultrasonic1 = 0,
	Ultrasonic2,
	Ultrasonic3,
	Ultrasonic4,
	Ultrasonic5,

	Size
};
}

namespace PinFunction {
enum Enum : uint8_t {
	Echo = 0,
	Trigger,

	None
};
}

class Ultrasonic {

	const Ultrasonics::Enum m_id;
	PinFunction::Enum m_mode;

	Util::Timer m_timer;
	Util::Timer m_pulseTimer;
	Util::Timer m_silenceTimer;
	Periph::SysTickCounter m_sysTick;

public:
	Ultrasonic(Ultrasonics::Enum id);

	void update();
	void configure(PinFunction::Enum fcn);

	void trigger();
	bool availbale();

	Container::Result<uint64_t> read();





};

}


#endif