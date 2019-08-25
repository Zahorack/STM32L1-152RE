/*
* Servo.cpp
*
*  Created on: 17.9. 2018
*      Author: Zahorack
*/

#include "Periph/Servo.h"
#include "Util/State.h"
#include "Util/Trace.h"

namespace Periph
{
	namespace States
	{
		enum Flags : uint8_t {
			IsRunning = 0x00,
		};
	}

	static Util::State<uint8_t> s_servoState;


	static uint16_t s_servoAngles[Servos::Size] = { 0, 0, 0, 0 };

	static Pwms::Enum servosToPwms(Servos::Enum id)
	{
		return static_cast<Pwms::Enum>(id + static_cast<Servos::Enum>(Pwms::Pwm5));
	}

	Servo::Servo(Servos::Enum id) :
		id(id),
		m_timer(Util::Time::FromMilliSeconds(1))
	{
		start();
		m_timer.start();
		m_pwm.write(servosToPwms(id), ServoConstant + ServoHomePosition);
		setTargetAngle(ServoHomePosition);
	}

	Servo::Servo(Servos::Enum id, int16_t defaultAngle) :
		id(id),
		m_timer(Util::Time::FromMilliSeconds(1))
	{
		start();
		m_timer.start();
		m_pwm.write(servosToPwms(id), ServoConstant + ServoHomePosition);
		setTargetAngle(ServoHomePosition + defaultAngle);
	}

	Servo::~Servo()
	{
		stop();
	}

	void Servo::start()
	{
		s_servoState.setFlag(id);
	}

	void Servo::stop()
	{
		s_servoState.resetFlag(id);
	}

	bool Servo::isRunning() const
	{
		return s_servoState.flag(id);
	}

	void Servo::setTargetAngle(uint16_t angle)
	{
		s_servoAngles[id] = ServoConstant + Util::clamp(angle, ServoMinPosition, ServoMaxPosition);
	}

	uint16_t Servo::getTargetAngle() const
	{
		return s_servoAngles[id];
	}

	uint16_t Servo::getCurrentAngle() const
	{
		return m_pwm.read(servosToPwms(id));
	}

	void Servo::incrementAngle()
	{
		setTargetAngle(getTargetAngle() + 1);
	}

	void Servo::decrementAngle()
	{
		setTargetAngle(getTargetAngle() - 1);
	}

	void Servo::addAngle(int16_t angle)
	{
		setTargetAngle(getTargetAngle() + angle);
	}

	void Servo::update()
	{
		if(isRunning() && m_timer.run()) {
			if(getCurrentAngle() < getTargetAngle())
				incrementAngleInternal();
			else if(getCurrentAngle() > getTargetAngle())
				decrementAngleInternal();
		}
	}

	void Servo::incrementAngleInternal()
	{
			m_pwm.write(servosToPwms(id), getCurrentAngle() + 1);
	}

	void Servo::decrementAngleInternal()
	{
			m_pwm.write(servosToPwms(id), getCurrentAngle() - 1);
	}
}