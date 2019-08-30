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
#include "Container/Queue.h"
#include "Util/Handshaking.h"
#include "Util/Timer.h"

namespace Control
{
	static constexpr uint16_t
		JostickQuadrantOffset = 70,
		JoystickMiddle = 100,
		JoystickTreshold = 2;

	class Communication {
		Periph::Usart m_rfModule;
		uint32_t m_transmitID = 0;
		uint32_t m_receiveID = 0;
		Util::Timer m_timer;

		enum State {
			WaitingForNextPacket,
			ReadingPacketHeader,
			ReadingPacketContents
		};

		State m_state = WaitingForNextPacket;
		Packet m_currentPacket;
		Packet m_transmitPacket;
		Util::Handshaking m_handshaking;

	public:
		Communication();

		Container::Result<Packet> update();
		void sendStatus();
		void sendAck();
		void sendNack();
		void send(PacketType::Enum type);

		void send(Packet);



	private:
		void waitForNextPacket();
		void readPacketHeader();
		Container::Result<Packet> readPacketContents();
		bool checkHeaderCrc();
		bool checkDataCrc();

		void sendHeader(PacketHeader header);
		void sendContents(PacketContents content);
	};
}
#endif /* UTIL_PACKET_H_ */