/**
  * @file    Application.cpp
  */

#include "Application.h"
#include "Util/Timer.h"
#include "Util/Trace.h"
#include "Periph/Ultrasonic.h"

LOGGER_MODULE(Application)

Application *Application::m_instance = nullptr;

Application::ApplicationInitializator::ApplicationInitializator(Application *parent)
{
    m_instance = parent;

}

Application::Application() :
	m_applicationInitializator(this),
	logger(&usartLog),
	usartLog(Periph::Usarts::Usart3, 576000),
	m_appRunningLed(Periph::Leds::Green),
	ultrasonic(Periph::Ultrasonics::Ultrasonic1)
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


	ultrasonic.trigger();

	/* @non-terminating@ */
	for(;;) {
//
//		Container::Result<Control::Packet> communicationResult = communication.update();
//
//		if(communicationResult.isValid && communicationResult.value.header.type == Control::PacketType::ManualControl)
//			control.setControlData(communicationResult.value.contents.dataPacket);
//
//		else if(communicationResult.isValid && communicationResult.value.header.type == Control::PacketType::ManualCalibration)
//			control.setCalibrationData(communicationResult.value.contents.calibrationPacket);
//
//		else if(communicationResult.isValid && communicationResult.value.header.type == Control::PacketType::OpenLeftFeeder)
//			control.command(Control::PacketType::OpenLeftFeeder);
//
//		else if(communicationResult.isValid && communicationResult.value.header.type == Control::PacketType::OpenRightFeeder)
//			control.command(Control::PacketType::OpenRightFeeder);
//
//		control.update();
//		bms.update();




        ultrasonic.update();



		if(timer.run()) {
            if(ultrasonic.available() == true) {
                Container::Result<Periph::ultrasonicResult_t> result = ultrasonic.read();

                for(int i = 0; i < Periph::UltrasonicWaves::Size; i++) {
                    TRACE("echo: %d   ", result.value.echoInterval[i]);
                    TRACE("pulse: %d \n", result.value.pulseInterval[i]);
                }
                TRACE("\n");
                ultrasonic.trigger();
            }


//            TRACE("echo: %d \n\r", ultrasonic.read().value);
//            ultrasonic.trigger();
			//communication.sendStatus();
			//communication.sendStatus();
			//INF_LOG("Tick");
			//TRACE("Tick\n\r");
		}

		static uint64_t last_micros = 0;
//
//		if(finetimer.read() > (last_micros + 100000)) {
//			TRACE("Second\n\r");
//			last_micros = finetimer.read();
//		}
	}
	INF_LOG("Application ended.");


}

Application *Application::instance()
{
	return m_instance;
}