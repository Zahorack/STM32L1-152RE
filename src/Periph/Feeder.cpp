/*
 * Feeder.cpp
 *
 *  Created on: 25.8.2019
 *      Author: Zahorack
 */


#include "Util/Trace.h"
#include "Util/Tool.h"
#include "Periph/Feeder.h"

namespace Periph {

static const uint16_t ServoOpenAngle = 600;

static const uint16_t OpenLeftAngle = Periph::ServoHomePosition + ServoOpenAngle;
static const uint16_t OpenRightAngle = Periph::ServoHomePosition - ServoOpenAngle;

Feeder::Feeder():
	m_servo(Periph::Servos::Servo1),
	m_state(FeederStates::Free)
{

}

void Feeder::open(Feeders::Enum id)
{
	if(id == Feeders::Left) {
		m_servo.start();
		m_state = FeederStates::BusyOpenLeft;
		m_servo.setTargetAngle(OpenLeftAngle);
	}
	else if(id == Feeders::Right) {
		m_servo.start();
		m_state = FeederStates::BusyOpenRight;
		m_servo.setTargetAngle(OpenRightAngle);
	}

}

void Feeder::update()
{
	if(m_state == FeederStates::BusyOpenLeft || m_state == FeederStates::BusyOpenRight) {
		if(m_servo.getCurrentAngle() == m_servo.getTargetAngle()) {
			m_servo.setTargetAngle(ServoHomePosition);
			m_state = FeederStates::Free;
		}
	}

	m_servo.update();
}

}