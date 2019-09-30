/*
 * Comunication.cpp
 *
 *  Created on: 2.9. 2018
 *      Author: Zahorack
 */

#include "Util/Communication.h"
#include "Util/Trace.h"
#include "Util/Bms.h"

//static Container::Queue<volatile Control::Packet, 10> s_priorityQueue;

namespace Util
{
	Communication::Communication() :
		m_rfModule(Periph::Usarts::Usart1, 57600),
		m_timer(Util::Time::FromSeconds(5))
	{
		m_timer.start();
	}

	Container::Result<Control::Packet> Communication::update()
	{
		//TRACE("rx: %d\n\r", m_rfModule.bytesAvailable());

		Container::Result<Control::Packet> attempt;

		if((attempt = m_handshaking.update()).isValid) {
			//TRACE("Handshaking send attempt\n\r");
			send(attempt.value);
		}

		if(m_timer.run() && m_state == WaitingForNextPacket) {
			sendStatus();
		}

		switch(m_state) {
			case WaitingForNextPacket:
				waitForNextPacket();
				break;

			case ReadingPacketHeader:
				readPacketHeader();
				break;

			case ReadingPacketContents:
				return readPacketContents();
		}

		return Container::Result<Control::Packet>();
	}


	void Communication::waitForNextPacket()
	{
		if(m_rfModule.bytesAvailable() > sizeof(Control::PacketMark)) {
			if(m_rfModule.read() == 0x4C && m_rfModule.read() == 0x4B) {
				m_state = ReadingPacketHeader;
				//TRACE("markOK\n\r");
			}
			else {
				TRACE("Wrong packet mark\n\r");
			}
		}
	}

	void Communication::readPacketHeader()
	{
		//TRACE("readPacketHeader\n\r");
		if(m_rfModule.bytesAvailable() >= sizeof(Control::PacketHeader) + sizeof(Control::Crc)) {
			m_rfModule.readStruct(m_currentPacket.header);

			if(checkHeaderCrc()) {
				TRACE("<- [%d]\n\r", m_currentPacket.header.type);
				m_state = ReadingPacketContents;
			}
			else {
				m_state = WaitingForNextPacket;
				TRACE("Header CRC ERROR\n\r");
			}
		}
	}

	Container::Result<Control::Packet> Communication::readPacketContents()
	{
		//TRACE("readPacketContents\n\r");
		if(Control::Packet::SizeForType(m_currentPacket.header.type) > 0) {
			if(m_rfModule.bytesAvailable() >= Control::Packet::SizeForType(m_currentPacket.header.type) + sizeof(Control::Crc)) {
				//m_rfModule.readStruct(m_currentPacket.contents);
				m_rfModule.readBytes(reinterpret_cast<uint8_t *>(&m_currentPacket.contents), Control::Packet::SizeForType(m_currentPacket.header.type));

				if(checkDataCrc()) {
					//TRACE("dataOK\n\r");
					if(m_currentPacket.header.type != Control::PacketType::ManualControl){
						sendAck();
					}
					m_state = WaitingForNextPacket;
					return Container::Result<Control::Packet>(m_currentPacket);
				}
			}
		}
		else {
			m_state = WaitingForNextPacket;
			if(m_currentPacket.header.type != Control::PacketType::Ack && m_currentPacket.header.type != Control::PacketType::Nack) {
				sendAck();
			}
			else if (m_currentPacket.header.type == Control::PacketType::Ack) {
				m_handshaking.check(m_currentPacket);
			}

			return Container::Result<Control::Packet>(m_currentPacket);
		}

		return Container::Result<Control::Packet>();
	}

	bool Communication::checkHeaderCrc()
	{
		Control::Crc crc = m_rfModule.read();

		if(crc != Control::Packet::CalculateCRC8(m_currentPacket.header)) {
			TRACE("HEADER CRC ERROR RX_CRC = %d    ", crc);
			TRACE("CRC = %d\n\r", Control::Packet::CalculateCRC8(m_currentPacket.header));
			sendNack();
			m_state = WaitingForNextPacket;
			return false;
		}
		return true;
	}

	bool Communication::checkDataCrc()
	{
		Control::Crc crc = m_rfModule.read();

		if(crc != Control::Packet::CalculateCRC8((uint8_t*)&m_currentPacket.contents, Control::Packet::SizeForType(m_currentPacket.header.type))) {
			TRACE("DATA CRC ERROR RX_CRC = %d    ", crc);
			TRACE("CRC = %d\n\r", Control::Packet::CalculateCRC8((uint8_t*)&m_currentPacket.contents.dataPacket.joystickData, Control::Packet::SizeForType(m_currentPacket.header.type)));
			//TRACE("size: %d\n\r", Control::Packet::SizeForType(m_currentPacket.header.type));
			//TRACE("data : %d", m_currentPacket.contents.dataPacket.joystickData.x);
			sendNack();
			m_state = WaitingForNextPacket;

			return false;
		}

		return true;
	}

	void Communication::sendHeader(Control::PacketHeader header)
	{
		m_rfModule.write((uint8_t*)&Control::PacketMark, 2);
		m_rfModule.writeStruct(header);
		m_rfModule.write(Control::Packet::CalculateCRC8(header));
	}


	 void Communication::sendContents(Control::PacketContents content, uint32_t size)
	{
		m_rfModule.write((uint8_t*)&content, size);
		m_rfModule.write(Control::Packet::CalculateCRC8((uint8_t*)&content, size));
	}


	void Communication::send(Control::Packet packet)
	{
		//TRACE("-> [%d]\n\r", packet.header.type);
		sendHeader(packet.header);
		sendContents(packet.contents, Control::Packet::SizeForType(packet.header.type));
	}

	void Communication::send(Control::PacketType::Enum type)
	{
		//TRACE("-> [%d]\n\r", type);
		Control::PacketHeader header = {
				.id = m_transmitID++,
				.type = type
		};
		sendHeader(header);
	}

	void Communication::sendAck()
	{
		TRACE("-> packet ACK[%d]\n\r", Control::PacketType::Ack);
		Control::PacketHeader ack = {
				.id = m_currentPacket.header.id,
				.type = Control::PacketType::Ack
		};
		sendHeader(ack);
	}

	void Communication::sendNack()
	{
		TRACE("-> packet NACK[%d]\n\r", Control::PacketType::Nack);
		Control::PacketHeader nack = {
				.id = m_currentPacket.header.id,
				.type = Control::PacketType::Nack
		};
		sendHeader(nack);
	}

	void Communication::sendStatus()
	{
		Control::Packet status;

		status.header.id = m_transmitID++;
		status.header.type = Control::PacketType::Status;

		status.contents.statusPacket.batteryChargeLevel = g_batteryChargeLevel;
		status.contents.statusPacket.uptime = 0;

		m_handshaking.add(status);

		send(status);

	}

}

