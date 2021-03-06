/*
 * VehicleMode.h
 *
 *  Created on: Nov 16, 2018
 *      Author: xgallom
 */

#ifndef MANUAL_MODE_H_
#define MANUAL_MODE_H_

#include "Util/Communication.h"
#include "Periph/Engine.h"
#include "Control/Packet.h"

namespace Control
{

	static constexpr uint16_t
		JostickQuadrantOffset = 70,
		JoystickMiddle = 100,
		JoystickTreshold = 2;

	class ManualMode {
		ManualCalibrationPacket m_calibration;

	public:
		ManualMode();
		void setControlData(
				const ManualControlPacket &controlData,
				Periph::Engine m_engine_left, Periph::Engine m_engine_right
				);

		void setCalibrationData(ManualCalibrationPacket &data);
	};
}

#endif /* MANUAL_MODE_H_ */