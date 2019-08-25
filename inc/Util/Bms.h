/*
 * Bms.h
 *
 *  Created on: 24.8.2019
 *      Author: Zahorack
 */

#ifndef BMS_H_
#define BMS_H_

#include "stm32l1xx.h"
#include "Periph/Adc.h"
#include "Util/Timer.h"
#include "Periph/Buzzer.h"

namespace Util {

class Bms {

	Periph::Adc m_adc;
	Util::Timer m_measTimer;
	Util::Timer m_checkTimer;
	Periph::Buzzer m_buzzer;

	float m_batteryVoltage;
	uint8_t m_batteryLevel;
	uint32_t m_uptime;

	float readBatteryVoltage();

public:
	Bms();

	void update();

	float getBatteryVoltage();
	uint8_t getChargeLevel();

};

}

#endif