/*
 * Application.h
 *
 *  Created on: Jun 17, 2018
 *      Author: xgallom
 */

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "stm32l1xx.h"
#include "stm32l1xx_nucleo.h"

#include "Periph/Led.h"
#include "Periph/Usart.h"
#include "Util/Logger.h"

#include "Control/Control.h"
#include "Util/Communication.h"

class Application {
	static Application *m_instance;

	class ApplicationInitializator {
	public:
		ApplicationInitializator(Application *parent);
	} m_applicationInitializator;


public:
	Util::Logger logger;
	Periph::Usart usartLog;

	Control::Control control;
	Control::Communication communication;

private:
	Periph::Led m_appRunningLed;



public:
	Application();

	void run();
	static Application *instance();

};

#define App ::Application::instance()

#endif /* APPLICATION_H_ */