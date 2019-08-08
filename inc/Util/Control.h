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
#include "Periph/Servo.h"
#include "Util/Packet.h"
#include "Util/Timer.h"
#include "Util/Tool.h"


#define	JOYSTICK_MIDDLE		100
#define	JOYSTICK_TRESHOLD	7


namespace Util {

class Control {

	Periph::Engine 	m_engine1, m_engine2;
	Periph::Servo 	m_servo;
	uint32_t 	m_disconnectedTime = 0;
	bool 		m_state = true;

public:
	Control();
	~Control();

	Periph::Usart rfModule;
	void update();
	void run();
	void stop();
	void start();

	void updateVehicleData();

	void updateEngines();


};

} /* namespace Util */

#endif /* UTIL_CONTROL_H_ */
