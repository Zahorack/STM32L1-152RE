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
	ManualMode::ManualMode()
	{
		m_calibration.directionCalibration = 0;
		m_calibration.maxPower = 90;
	}

	void ManualMode::setControlData(
			const ManualControlPacket &controlData,
			Periph::Engine m_engine_left, Periph::Engine m_engine_right
			)
	{
//		TRACE("y: %d", controlData.joystickData.y);
//		TRACE("x: %d \n\r", controlData.joystickData.x);

		uint8_t leftSpeed = 0, rightSpeed = 0;

		if(controlData.joystickData.y > JoystickMiddle + JoystickTreshold) {
			m_engine_left.setTargetDirection(Periph::Dirs::Backward);
			leftSpeed = controlData.joystickData.y - JoystickMiddle + JoystickTreshold;
		}
		else if(controlData.joystickData.y < JoystickMiddle - JoystickTreshold) {
			m_engine_left.setTargetDirection(Periph::Dirs::Forward);
			leftSpeed = JoystickMiddle - controlData.joystickData.y - JoystickTreshold;
		}

		if(controlData.joystickData.x > JoystickMiddle + JoystickTreshold) {
			m_engine_right.setTargetDirection(Periph::Dirs::Backward);

			rightSpeed = controlData.joystickData.x - JoystickMiddle + JoystickTreshold;
		}
		else if(controlData.joystickData.x < JoystickMiddle - JoystickTreshold) {
			m_engine_right.setTargetDirection(Periph::Dirs::Forward);
			rightSpeed = JoystickMiddle - controlData.joystickData.x  - JoystickTreshold;
		}


		leftSpeed = Util::clamp<uint8_t>(Util::map<uint8_t>(leftSpeed, 0, 100, 50, m_calibration.maxPower - m_calibration.directionCalibration), 0, 100);
		rightSpeed = Util::clamp<uint8_t>(Util::map<uint8_t>(rightSpeed, 0, 100, 50, m_calibration.maxPower + m_calibration.directionCalibration), 0, 100);

		m_engine_left.setTargetSpeed(leftSpeed > 55 ? leftSpeed : 0);
		m_engine_right.setTargetSpeed(rightSpeed > 55 ? rightSpeed : 0);

		m_engine_left.start();
		m_engine_right.start();

	}

	void ManualMode::setCalibrationData(ManualCalibrationPacket &data)
	{
		if(Util::inRange<int8_t>(data.directionCalibration, -100, 100) &&
			Util::inRange<uint8_t>(data.maxPower, 0, 100))
		{
			memcpy(&m_calibration, &data, sizeof(ManualCalibrationPacket));
			TRACE("L:%d\n\r", m_calibration.directionCalibration);
			TRACE("P:%d\n\r", m_calibration.maxPower);
		}
	}
}