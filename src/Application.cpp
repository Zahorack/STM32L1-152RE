/**
  * @file    Application.cpp
  */

#include "Application.h"
#include "Util/Timer.h"
#include "Util/Trace.h"
#include "Periph/Ultrasonic.h"
#include "Control/Packet.h"

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

    communication.sendStatus();

	Util::Timer timer(Util::Time::FromMilliSeconds(1000));
	timer.start();
	int main_cnt = 0;

	/* @non-terminating@ */
	for(;;) {

		Container::Result<Control::Packet> communicationResult = communication.update();

        if(communicationResult.isValid) {
            if (communicationResult.value.header.type == Control::PacketType::ManualControl)
                control.setControlData(communicationResult.value.contents.dataPacket);

            else if (communicationResult.value.header.type == Control::PacketType::ManualCalibration)
                control.setCalibrationData(communicationResult.value.contents.calibrationPacket);

            else if (communicationResult.value.header.type == Control::PacketType::OpenLeftFeeder)
                control.command(Control::PacketType::OpenLeftFeeder);

            else if (communicationResult.value.header.type == Control::PacketType::OpenRightFeeder)
                control.command(Control::PacketType::OpenRightFeeder);
        }

		control.update();
		bms.update();


		sonar.update();
		if(timer.run()) {

            if(sonar.singleBeamUltrasonic.available()) {
                Container::Result<Periph::jsnsr04Result_t> res = sonar.singleBeamUltrasonic.read();
                TRACE("single beam: %d\n\r", res.value.echoInterval);

                Control::SingleBeamSonarData data = {
                        .echoInterval =  res.value.echoInterval
                };
                communication.sendSingleBeamSonarData(data);

            }


            if(sonar.available(Periph::Ultrasonics::Ultrasonic4)) {
                Container::Result<Periph::ultrasonicResult_t> result = sonar.read(Periph::Ultrasonics::Ultrasonic4);
                for (int i = 0; i < Periph::UltrasonicWaves::Size; i++) {
                    TRACE("echo: %d   ", result.value.echoInterval[i]);
                    TRACE("pulse: %d \n", result.value.pulseInterval[i]);
                }
                TRACE(" closest: %d   ", result.value.echoInterval[result.value.closestEchoIndex]);
                TRACE("strongest: %d \n", result.value.echoInterval[result.value.strongestEchoIndex]);
                TRACE("\n");
            }
            sonar.trigger(Periph::Ultrasonics::Ultrasonic4);

//		    if (sonar.available()) {
//		        Container::Result<Util::sonarResult_t> result = sonar.read();
//
//                for (int i = 0; i < Periph::Ultrasonics::Size; i++) {
//                    uint32_t strongestIndex = result.value.ultrasonicsResults[i].value.strongestEchoIndex;
//                    uint32_t closestIndex = result.value.ultrasonicsResults[i].value.closestEchoIndex;
//
//                    TRACE("[%d] closest: %d   ",i,  result.value.ultrasonicsResults[i].value.echoInterval[closestIndex]);
//                    TRACE("strongest: %d \n", result.value.ultrasonicsResults[i].value.echoInterval[strongestIndex]);
//                }
//                TRACE("\n\r");
//		    }
//		    sonar.sequencedTrigger();

		}



//			//communication.sendStatus();
//			//communication.sendStatus();
//			//INF_LOG("Tick");
//			//TRACE("Tick\n\r");

        }
        INF_LOG("Application ended.");


}

Application *Application::instance()
{
	return m_instance;
}