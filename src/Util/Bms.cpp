/*
 * Bms.cpp
 *
 *  Created on: 24.8.2019
 *      Author: Zahorack
 */


#include "Util/Trace.h"
#include "Util/Bms.h"
#include "Util/Tool.h"

uint8_t g_batteryChargeLevel;

namespace Util {

static float KalmanFilter(float z_measured);

namespace Battery {
/* 	3s Lithium polymer
 * 	50C -->> 250A discahrge current
 * 	5000mAh // 55Wh
 */
	static const float FullChargeVoltage = 12.6;
	static const float AlarmChargeVoltage = 11.0;
	static const float EmptyChargeVoltage = 10.0;

	enum : uint8_t {
		Level0 = 0,
		Level10,
		Level20,
		Level30,
		Level40,
		Level50,
		Level60,
		Level70,
		Level80,
		Level90,
		Level95,
		Level100,

		Size
	};

	const float DischargeTable[Size] = {
		EmptyChargeVoltage,
		11.06,
		11.18,
		11.30,
		11.39,
		11.51,
		11.62,
		11.86,
		12.07,
		12.33,
		12.45,
		FullChargeVoltage
	};
}

static const float VoltageCorectionConstant = 1.01;

namespace VoltageDivider {
	static const uint16_t R1 = 30; //kohm
	static const uint16_t R2 = 10; //kohm
}

uint8_t Bms::getChargeLevel()
{
	uint8_t level = 0;

	if(Util::inRange<float>(m_batteryVoltage, Battery::DischargeTable[Battery::Level0],
			Battery::DischargeTable[Battery::Level10])) {
		level = 0;
	}
	else if(Util::inRange<float>(m_batteryVoltage, Battery::DischargeTable[Battery::Level10],
			Battery::DischargeTable[Battery::Level20])) {
		level = 10;
	}
	else if(Util::inRange<float>(m_batteryVoltage, Battery::DischargeTable[Battery::Level20],
			Battery::DischargeTable[Battery::Level30])) {
		level = 20;
	}
	else if(Util::inRange<float>(m_batteryVoltage, Battery::DischargeTable[Battery::Level30],
			Battery::DischargeTable[Battery::Level40])) {
		level = 30;
	}
	else if(Util::inRange<float>(m_batteryVoltage, Battery::DischargeTable[Battery::Level40],
			Battery::DischargeTable[Battery::Level50])) {
		level = 40;
	}
	else if(Util::inRange<float>(m_batteryVoltage, Battery::DischargeTable[Battery::Level50],
			Battery::DischargeTable[Battery::Level60])) {
		level = 50;
	}
	else if(Util::inRange<float>(m_batteryVoltage, Battery::DischargeTable[Battery::Level60],
			Battery::DischargeTable[Battery::Level70])) {
		level = 60;
	}
	else if(Util::inRange<float>(m_batteryVoltage, Battery::DischargeTable[Battery::Level70],
			Battery::DischargeTable[Battery::Level80])) {
		level = 70;
	}
	else if(Util::inRange<float>(m_batteryVoltage, Battery::DischargeTable[Battery::Level80],
			Battery::DischargeTable[Battery::Level90])) {
		level = 80;
	}
	else if(Util::inRange<float>(m_batteryVoltage, Battery::DischargeTable[Battery::Level90],
			Battery::DischargeTable[Battery::Level95])) {
		level = 90;
	}
	else if(Util::inRange<float>(m_batteryVoltage, Battery::DischargeTable[Battery::Level95],
			Battery::DischargeTable[Battery::Level100])) {
		level = 100;
	}
	else if(m_batteryVoltage > Battery::DischargeTable[Battery::Level100]) {
		level = 100;
	}

	return level;
}

Bms::Bms():
	m_adc(Periph::AdcPins::AdcPin1),
	m_measTimer(Util::Time::FromMilliSeconds(200)),
	m_checkTimer(Util::Time::FromSeconds(5)),
	m_buzzer(Periph::Buzzers::Buzzer1),
	m_batteryVoltage(0),
	m_batteryLevel(0),
	m_uptime(0)

{
	m_measTimer.start();
	m_checkTimer.start();
	//m_buzzer.start();
}


float Bms::readBatteryVoltage()
{
	float dividerVoltage = m_adc.read()*(3.3/4096.0);
	float batteryVoltage = dividerVoltage*((VoltageDivider::R1 + VoltageDivider::R2)/VoltageDivider::R2);

	return batteryVoltage * VoltageCorectionConstant;
}

float Bms::getBatteryVoltage()
{
	return m_batteryVoltage;
}

void Bms::update()
{
	if(m_measTimer.run()) {
		m_batteryVoltage = /*KalmanFilter*/(readBatteryVoltage());
	}

	if(m_checkTimer.run()) {
		m_uptime++;
		TRACE("Battery charge level: %d / voltage : %f\n\r", getChargeLevel(), readBatteryVoltage());

		if(getChargeLevel() == 0 && m_uptime > 5) {
			m_buzzer.start();
		}
		else {
			m_buzzer.stop();
		}
	}

	g_batteryChargeLevel = getChargeLevel();
	m_buzzer.update();
}



static float KalmanFilter(float z_measured) {
            static float x_est_last = 0;
            static float P_last = 0;
            float K;
            float P;
            float P_temp;
            float x_temp_est;
            float x_est;
            float Q = 0.025;
            float R = 0.7;

            x_temp_est = x_est_last;
            P_temp = P_last + Q;

            K = P_temp * (1.0/(P_temp + R));

            x_est = x_temp_est + K * ((float)z_measured - x_temp_est);
            P = (1- K) * P_temp;

            P_last = P;
            x_est_last = x_est;

            return (float)x_est;
        }

}