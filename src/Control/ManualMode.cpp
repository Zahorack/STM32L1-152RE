/*
 * VehicleMode.cpp
 *
 *  Created on: Nov 16, 2018
 *      Author: xgallom
 */

#include "Control/ManualMode.h"
#include "Util/Tool.h"
#include "Util/Trace.h"


namespace Control
{
	void ManualMode::setControlData(
			const ManualControlPacket &controlData,
			Periph::Engine m_engine_left, Periph::Engine m_engine_right
			)
	{
//		TRACE("x: %d", controlData.joystickData.x);
//		TRACE("y: %d \n\r", controlData.joystickData.y);

		uint8_t leftSpeed = 0, rightSpeed = 0;

		if(controlData.joystickData.y > JoystickMiddle + JoystickTreshold) {
			m_engine_left.setTargetDirection(Periph::Dirs::Forward);
			leftSpeed = controlData.joystickData.y - JoystickMiddle;
		}
		else if(controlData.joystickData.y < JoystickMiddle - JoystickTreshold) {
			m_engine_left.setTargetDirection(Periph::Dirs::Backward);
			leftSpeed = JoystickMiddle - controlData.joystickData.y;
		}

		if(controlData.joystickData.x > JoystickMiddle + JoystickTreshold) {
			m_engine_right.setTargetDirection(Periph::Dirs::Backward);
			rightSpeed = controlData.joystickData.x - JoystickMiddle;
		}
		else if(controlData.joystickData.x < JoystickMiddle - JoystickTreshold) {
			m_engine_right.setTargetDirection(Periph::Dirs::Forward);
			rightSpeed = JoystickMiddle - controlData.joystickData.x;
		}

//		m_engine_left.setTargetSpeed(Util::clamp<uint8_t>(leftSpeed, 0, 100));
//		m_engine_right.setTargetSpeed(Util::clamp<uint8_t>(rightSpeed, 0, 100));

		m_engine_left.start();
		m_engine_right.start();

	}
}