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
		m_engine_right(Periph::Engines::M2),
		m_servo(Periph::Servos::Servo1)
	{
		m_watchdog.start();
		m_updateTimer.start();

		stop();

	}

	Control::~Control(){
		stop();
	}

	void Control::stop()
	{
		m_engine_left.stop();
		m_engine_right.stop();
		m_servo.stop();
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

	void Control::update()
	{
		if(m_watchdog.run()) {
			stop();
			INF_LOG("DISCONNECTED");
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
		m_servo.update();

	}
}