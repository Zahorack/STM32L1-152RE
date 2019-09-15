/*
 * Control.h
 *
 *  Created on: 2.9 2018
 *      Author: Zahorack
 */

#ifndef UTIL_CONTROL_H_
#define UTIL_CONTROL_H_

#include "stm32l1xx.h"
#include "Periph/Engine.h"
#include "Periph/Usart.h"
#include "Periph/Feeder.h"

#include "Control/ManualMode.h"
#include "Util/Communication.h"
#include "Control/Packet.h"

#include "Util/Timer.h"
#include "Container/Queue.h"

namespace Control
{
	namespace ControlModes
	{
		enum Enum : uint8_t {
			ManualMode = 0,

			Stopped,

			Size
		};
	}

	class Control {
		Periph::Feeder m_feeder;
		Periph::Engine m_engine_left, m_engine_right;


		Util::Timer
			m_watchdog = Util::Timer(Util::Time::FromSeconds(1)),
			m_updateTimer = Util::Timer(Util::Time::FromMilliSeconds(10));

		ManualMode m_manualControlMode;

		ControlModes::Enum m_controlMode = ControlModes::ManualMode;

	public:
		Control();
		~Control();

		void stop();

		void setControlData(const ManualControlPacket &controlData);
		void setCalibrationData(ManualCalibrationPacket &calibrationData);
		void command(PacketType::Enum cmd);

		void update();
	};
}

#endif /* UTIL_CONTROL_H_ */