/**
  * @file    Application.cpp
  */

#include "Application.h"
#include "Util/Timer.h"
#include "Util/Trace.h"
LOGGER_MODULE(Application)

Application *Application::m_instance = nullptr;

Application::ApplicationInitializator::ApplicationInitializator(Application *parent)
{
	m_instance = parent;

}

Application::Application() :
	m_applicationInitializator(this),
	logger(&usartLog),
	usartLog(Periph::Usarts::Usart1, 9600),
	m_engine(Periph::Engines::M1),
	m_appRunningLed(Periph::Leds::Green)
{
	HAL_Init();
	SystemCoreClockUpdate();

}

void Application::run()
{
	INF_LOG("Application started running.");

	m_appRunningLed.turnOn();

//m_engine.setTargetSpeed(80);

	Util::Timer timer(Util::Time::FromMilliSeconds(100));
	timer.start();

	/* @non-terminating@ */
	for(;;) {

		m_engine.update();
		//TRACE("a");
		if(timer.run()) {
			m_appRunningLed.toggle();
			usartLog.write('c');
		}

		//usartLog.write('c');

	}

	INF_LOG("Application ended.");


}

Application *Application::instance()
{
	return m_instance;
}