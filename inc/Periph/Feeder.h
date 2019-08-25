/*
 * Feeder.h
 *
 *  Created on: 25.8.2019
 *      Author: Zahorack
 */

#ifndef FEEDER_H_
#define FEEDER_H_

#include "stm32l1xx.h"
#include "Util/Timer.h"
#include "Periph/Servo.h"

namespace Periph {

namespace Feeders {
enum Enum : uint8_t {
	Right = 0,
	Left,

	Size
};
}

namespace FeederStates {
enum : uint8_t {
	BusyOpenLeft = 0,
	BusyOpenRight,
	Free
};
}

class Feeder {

	Periph::Servo m_servo;
	uint8_t m_state;
public:
	Feeder();

	void open(Feeders::Enum id);
	void update();
};

}

#endif