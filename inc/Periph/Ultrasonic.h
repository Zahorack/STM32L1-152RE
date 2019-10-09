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

namespace UltrasonicStates {
enum Enum : uint8_t {
	Echo = 0,
	Trigger,
	Silent,

	None
};
}

    typedef struct {
        uint32_t echoTime;
        uint32_t pulseTime;
    }ultrasonicResult_t;

class Ultrasonic {

	const Ultrasonics::Enum m_id;
    UltrasonicStates::Enum m_state;

	Util::Timer m_timer;
	Periph::SysTickCounter m_sysTick;

public:
	Ultrasonic(Ultrasonics::Enum id);

	void update();
	void configure(UltrasonicStates::Enum fcn);

	void trigger();
	bool availbale();

	Container::Result<ultrasonicResult_t> read();


};

}


#endif