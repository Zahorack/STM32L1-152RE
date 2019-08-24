/*
 * Engine.cpp
 *
 *  Created on: Jun 18, 2018
 *      Author: Zahorack
 */

#include "Periph/Engine.h"
#include "Util/State.h"

namespace Periph {

namespace States {
enum Flags : uint8_t {
	IsRunning = 0x00,
	IsBackward,

	Offset
};
}

Util::State<uint16_t> s_engineState;
uint8_t s_engineSpeeds[Engines::Size] = { 0, 0, 0, 0, 0, 0 };

static Pwms::Enum enginesToPwms(Engines::Enum id)
{
	return static_cast<Pwms::Enum>(id);
}

static Dirs::Enum pinToDirection(bool pinState)
{
	return pinState ? Dirs::Backward : Dirs::Forward;
}

static bool directionToPin(Dirs::Enum direction)
{
	return direction == Dirs::Backward ? true : false;
}

static constexpr struct {
	GPIO_TypeDef *port;
	uint16_t id;
} DirPinsConfig[Engines::Size]= {
 	/* M1 */
	{ /* Dir pin 1 */
			port: GPIOA,
			id: GPIO_PIN_4
	},
	/* M2 */
	{ /* Dir pin 2 */
			port: GPIOB,
			id: GPIO_PIN_0
	}
};


Engine::Engine(Engines::Enum id) :
	id(id),
	m_pwm(),
	m_timer(Util::Time::FromMilliSeconds(1)),
	m_direction(DirPinsConfig[id].port, DirPinsConfig[id].id)
{
	m_pwm.write(enginesToPwms(id), 0);
	setCurrentDirection(Dirs::Forward);
	m_timer.start();
}

void Engine::start()
{
	s_engineState.setFlag(States::IsRunning + (id << States::Offset));
}

void Engine::stop()
{
	s_engineState.resetFlag(States::IsRunning + (id << States::Offset));
}

bool Engine::isRunning() const
{
	return s_engineState.flag(States::IsRunning + (id << States::Offset));
}

void Engine::setTargetSpeed(uint8_t speed)
{
	s_engineSpeeds[id] = speed;
}

void Engine::setCurrentSpeed(uint8_t speed)
{
	m_pwm.write(enginesToPwms(id), speed);
}

uint8_t Engine::getTargetSpeed() const
{
	return s_engineSpeeds[id];
}

uint8_t Engine::getCurrentSpeed() const
{
	return m_pwm.read(enginesToPwms(id));
}

void Engine::setTargetDirection(Dirs::Enum direction)
{
	s_engineState.setFlagTo(States::IsBackward + States::Offset * id, directionToPin(direction));
}

void Engine::setCurrentDirection(Dirs::Enum direction)
{
	m_direction.setPinTo(directionToPin(direction));
}

Dirs::Enum Engine::getTargetDirection() const
{
	return pinToDirection(s_engineState.flag(States::IsBackward + States::Offset * id));
}

Dirs::Enum Engine::getCurrentDirection() const
{
	return pinToDirection(m_direction.readPin());
}

void Engine::speedUp()
{
	m_pwm.write(enginesToPwms(id), getCurrentSpeed() + 1);
}

void Engine::slowDown()
{
	m_pwm.write(enginesToPwms(id), getCurrentSpeed() - 1);
}

void Engine::turnAround()
{
	if(getCurrentSpeed() != 0x00)
		slowDown();
	else
		setCurrentDirection(getTargetDirection());
}

void Engine::moveInDirection()
{
	if(getCurrentSpeed() < getTargetSpeed())
		speedUp();
	else if(getCurrentSpeed() > getTargetSpeed())
		slowDown();
}

void Engine::update()
{
	//if(m_timer.run()) {
		if(isRunning()) {
			if(getCurrentDirection() != getTargetDirection())
				turnAround();
			else
				moveInDirection();
		}
	//}
}

} /* namespace Periph */
