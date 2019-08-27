/*
 * Comunication.cpp
 *
 *  Created on: 2.9. 2018
 *      Author: Zahorack
 */

#include "Util/Communication.h"
#include "Util/Trace.h"
#include "Util/Bms.h"

namespace Control
{
	Communication::Communication() :
		m_rfModule(Periph::Usarts::Usart1, 57600)
	{}

	Container::Result<Packet> Communication::update()
	{
		//TRACE("rx: %d\n\r", m_rfModule.bytesAvailable());
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

		return Container::Result<Packet>();
	}


	void Communication::waitForNextPacket()
	{
		while(m_rfModule.bytesAvailable() > sizeof(PacketMark)) {
			if(m_rfModule.readWord() == PacketMark) {
				m_state = ReadingPacketHeader;
				break;
			}
		}
	}

	void Communication::readPacketHeader()
	{
		if(m_rfModule.bytesAvailable() >= sizeof(PacketHeader) + sizeof(Crc)) {
			m_rfModule.readStruct(m_currentPacket.header);

			if(checkHeaderCrc()) {
				//TRACE("Header OK type[%d]\n\r", m_currentPacket.header.type);
				m_state = ReadingPacketContents;
			}
			else {
				m_state = WaitingForNextPacket;
				TRACE("Header CRC ERROR\n\r");
			}
		}
	}

	Container::Result<Packet> Communication::readPacketContents()
	{
		if(Packet::SizeForType(m_currentPacket.header.type) > 0) {
			if(m_rfModule.bytesAvailable() >= Packet::SizeForType(m_currentPacket.header.type) + sizeof(Crc)) {
				//m_rfModule.readStruct(m_currentPacket.contents);
				m_rfModule.readBytes(reinterpret_cast<uint8_t *>(&m_currentPacket.contents), Packet::SizeForType(m_currentPacket.header.type));

				if(checkDataCrc()) {
					sendAck();
					m_state = WaitingForNextPacket;
					return Container::Result<Packet>(m_currentPacket);
				}
			}
		}
		else {
			m_state = WaitingForNextPacket;
			sendAck();
			return Container::Result<Packet>(m_currentPacket);
		}

		return Container::Result<Packet>();
	}

	bool Communication::checkHeaderCrc()
	{
		Crc crc = m_rfModule.read();

		if(crc != Packet::CalculateCRC8(m_currentPacket.header)) {
			sendNack();
			m_state = WaitingForNextPacket;

			return false;
		}

		return true;
	}

	bool Communication::checkDataCrc()
	{
		Crc crc = m_rfModule.read();

		if(crc != Packet::CalculateCRC8(m_currentPacket.contents.dataPacket)) {
			TRACE("DATA CRC ERROR -- RX CRC = %d   ", crc);
			TRACE("CRC = %d\n\r", Packet::CalculateCRC8(m_currentPacket.contents.dataPacket));

			sendNack();
			m_state = WaitingForNextPacket;

			return false;
		}

		return true;
	}

	void Communication::sendHeader(PacketHeader header)
	{

		m_rfModule.write((uint8_t*)&PacketMark, 2);
		m_rfModule.writeStruct(header);
		m_rfModule.write(Packet::CalculateCRC8(header));
	}

	void Communication::sendContents(PacketContents content)
	{
		m_rfModule.writeStruct(content);
		m_rfModule.write(Packet::CalculateCRC8(content));
	}

	void Communication::send(Packet packet)
	{

		sendHeader(packet.header);
		sendContents(packet.contents);
	}

	void Communication::send(PacketType::Enum type)
	{
		PacketHeader header = {
				.id = m_transmitID++,
				.type = type
		};
		sendHeader(header);
	}

	void Communication::sendAck()
	{
		PacketHeader ack = {
				.id = m_currentPacket.header.id,
				.type = PacketType::Ack
		};
		sendHeader(ack);
	}

	void Communication::sendNack()
	{
		PacketHeader nack = {
				.id = m_currentPacket.header.id,
				.type = PacketType::Nack
		};
		sendHeader(nack);
	}

	void Communication::sendStatus()
	{
		Packet status;

		status.header.id = m_transmitID++;
		status.header.type = PacketType::Status;

		status.contents.statusPacket.batteryChargeLevel = g_batteryChargeLevel;
		status.contents.statusPacket.uptime = 0;

		send(status);

	}

}

