/*
 * Servo.h
 *
 *  Created on: 17.9. 2018
 *      Author: Zahorack
 */

#ifndef PERIPH_SERVO_H_
#define PERIPH_SERVO_H_

#include <Periph/DigitalOutputPin.h>
#include <Util/Tool.h>
#include "stm32l1xx.h"
#include "Pwm.h"
#include "Util/Timer.h"

namespace Periph
{

	/* Servo  specification
	 *
	 * Left position   s_servoConstant
	 * Home position:  s_servoConstant + 1000
	 * Right position  s_servoConstant + 2000
	 *
	 * Working resolution: +-1000 samples
	 */
	const uint16_t
		ServoConstant = 500,
		ServoHomePosition = 1000,
		ServoMaxPosition = 2000,
		ServoMinPosition = 0;

	namespace Servos
	{
		enum Enum : uint8_t {
			Servo1 = 0,
			Servo2,
			Servo3,
			Servo4,

			Size
		};
	}

	class Servo {
		const Servos::Enum id;

		Pwm m_pwm;
		Util::Timer m_timer;

	public:
		Servo(Servos::Enum id);
		Servo(Servos::Enum id, int16_t defaultAngle);
		~Servo();

		void start();
		void stop();
		bool isRunning() const;

		void setTargetAngle(uint16_t angle);
		uint16_t getTargetAngle() const;
		uint16_t getCurrentAngle() const;

		void incrementAngle();
		void decrementAngle();

		void addAngle(int16_t angle);

		void update();

	private:
		void incrementAngleInternal();
		void decrementAngleInternal();
	};
}

#endif /* PERIPH_SERVO_H_ */