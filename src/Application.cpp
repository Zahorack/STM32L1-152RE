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
	usartLog(Periph::Usarts::Usart3, 9600),
	m_appRunningLed(Periph::Leds::Green)
{
	HAL_Init();
	SystemCoreClockUpdate();
}

void Application::run()
{
	m_appRunningLed.turnOn();
	INF_LOG("Application started running.");

	//communication.sendStatus();

	Util::Timer timer(Util::Time::FromMilliSeconds(1000));
	timer.start();

	/* @non-terminating@ */
	for(;;) {

		Container::Result<Control::Packet> communicationResult = communication.update();

		if(communicationResult.isValid && communicationResult.value.header.type == Control::PacketType::ManualControl)
			control.setControlData(communicationResult.value.contents.dataPacket);

		control.update();
		bms.update();

		if(timer.run()) {
			//communication.sendStatus();
			//INF_LOG("Tick");
			//TRACE("Tick\n\r");
		}

	}

	INF_LOG("Application ended.");


}

Application *Application::instance()
{
	return m_instance;
}