/*
 * Comunication.h
 *
 *  Created on: 2.9. 2018
 *      Author: Zahorack
 */

#ifndef UTIL_COMMUNICATION_H_
#define UTIL_COMMUNICATION_H_

#include <Container/Result.h>
#include "stm32l1xx.h"
#include "Periph/Usart.h"
#include "Control/Packet.h"

namespace Control
{
	static constexpr uint16_t
		JostickQuadrantOffset = 70,
		JoystickMiddle = 100,
		JoystickTreshold = 2;

	class Communication {
		Periph::Usart m_rfModule;

		enum State {
			WaitingForNextPacket,
			ReadingPacketHeader,
			ReadingPacketContents
		};

		State m_state = WaitingForNextPacket;
		Packet m_currentPacket;

	public:
		Communication();

		Container::Result<Packet> update();
		void sendStatus();

	private:
		void waitForNextPacket();
		void readPacketHeader();
		Container::Result<Packet> readPacketContents();
		bool checkHeaderCrc();
		bool checkDataCrc();
	};
}
#endif /* UTIL_PACKET_H_ */