/*
 * Control.cpp
 *
 *  Created on: Jul 23, 2018
 *      Author: Zahorack
 */

#include "Control/Control.h"
#include "Application.h"
#include "Util/Tool.h"
#include "Util/Trace.h"


LOGGER_MODULE(Control)

namespace Control
{
	Control::Control():
		m_engine_left(Periph::Engines::M1),
		m_engine_right(Periph::Engines::M2)
	{
		m_watchdog.start();
		m_updateTimer.start();

		stop();
		//m_feeder.open(Periph::Feeders::Left);
	}

	Control::~Control(){
		stop();
	}

	void Control::stop()
	{
		m_engine_left.stop();
		m_engine_right.stop();
	}

	void Control::setControlData(const ManualControlPacket &controlData)
	{
		m_watchdog.wasTriggeredNow();

		switch(m_controlMode) {
			case ControlModes::ManualMode:
				m_manualControlMode.setControlData(controlData, m_engine_left, m_engine_right);
				break;


			case ControlModes::Stopped:
				break;
			case ControlModes::Size:
				break;
		}
	}

	void Control::command(PacketType::Enum cmd)
	{
		TRACE("cmd : %d", cmd);
		switch((uint8_t)cmd) {
			case PacketType::OpenLeftFeeder : m_feeder.open(Periph::Feeders::Left); break;
			case PacketType::OpenRightFeeder : m_feeder.open(Periph::Feeders::Right); break;
		}
	}

	void Control::update()
	{
		if(m_watchdog.run()) {
			stop();
			//INF_LOG("DISCONNECTED");
		}

		switch(m_controlMode) {
		case ControlModes::ManualMode:
			break;

		case ControlModes::Stopped:
			break;
		case ControlModes::Size:
			break;
		}

		m_engine_left.update();
		m_engine_right.update();
		m_feeder.update();
	}
}