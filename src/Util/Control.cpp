/*
 * Control.cpp
 *
 *  Created on: Jul 23, 2018
 *      Author: Zahorack
 */

#include <Util/Trace.h>
#include "Util/Control.h"
#include "Application.h"
#include "Util/Timer.h"

namespace Util {

LOGGER_MODULE(Application)

enum modes : uint8_t{
  vehicle_mode = 0,
  sunTracker_mode,
  printing_mode,
  simulation_mode,

  modes_size
  };


Control::Control():
	m_engine1(Periph::Engines::M1),
	m_engine2(Periph::Engines::M2),
	m_servo(Periph::Servos::Servo1),
	rfModule(Periph::Usarts::Usart1, 9600)

{
//	m_servo.start();
//	m_servo2.addAngle(60);   //korekcia pociatocnej polohy
//	stop();

}

Control::~Control(){
	stop();
}


void Control::updateEngines(){
	//m_engine1.update();
	//m_engine2.update();
}



void Control::stop(){
		//m_servo1.stop();
		m_state = false;
}

void Control::start(){
		///m_servo1.start();
		m_state = true;
}

void Control::run(){


//	if(m_watchdog.run()){
//		m_disconnectedTime++;
//	}

//	if(rfModule.Available()) {
//
//		rfModule.read();
//	}

//	if(rfModule.Available())
//		if(rfModule.bytesAvailable() >= sizeof(ctrlData) +1) {
//			rfModule.readBytesUntil(';', (uint8_t *)&ctrlData, sizeof(ctrlData) +1);
//
//			dataOK = false;
//			uint8_t dataCRC = m_packet.calc_crc8((uint8_t *)&ctrlData, sizeof(ctrlData) -1);
//
//			if(ctrlData.data_crc == dataCRC){
//				m_disconnectedTime = 0;
//				dataOK = true;
//
//				if(ctrlData.mode != s_mode)
//					;//switchMode();
//
//				TRACE("right: %d  ",ctrlData.x);
//				TRACE("left: %d  ",ctrlData.y);
//				TRACE("state: %d  ",ctrlData.state);
////				TRACE("button L: %d  ",ctrlData.button_left);
////				TRACE("button R: %d  ",ctrlData.button_right);
////				TRACE("mode: %d  ",ctrlData.mode);
//				TRACE("POT: %d \r\n",ctrlData.pot);
//			}
//
//		}
}


void Control::update(){

	updateEngines();
}


} /* namespace Util */
