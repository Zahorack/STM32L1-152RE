/*
 * Comunication.cpp
 *
 *  Created on: 2.9. 2018
 *      Author: Zahorack
 */

#include "Util/Communication.h"
#include "Util/Trace.h"
#include "Util/Bms.h"

static Container::Queue<volatile Control::Packet, 10> s_priorityQueue;

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
		if(m_rfModule.bytesAvailable() > sizeof(PacketMark)) {
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
		if(m_rfModule.bytesAvailable() >= sizeof(PacketHeader) + sizeof(Crc)) {
			m_rfModule.readStruct(m_currentPacket.header);

			if(checkHeaderCrc()) {
				TRACE("<- packet type[%d]\n\r", m_currentPacket.header.type);
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
		//TRACE("readPacketContents\n\r");
		if(Packet::SizeForType(m_currentPacket.header.type) > 0) {
			if(m_rfModule.bytesAvailable() >= Packet::SizeForType(m_currentPacket.header.type) + sizeof(Crc)) {
				//m_rfModule.readStruct(m_currentPacket.contents);
				m_rfModule.readBytes(reinterpret_cast<uint8_t *>(&m_currentPacket.contents), Packet::SizeForType(m_currentPacket.header.type));

				if(checkDataCrc()) {
					//TRACE("dataOK\n\r");
					if(m_currentPacket.header.type != PacketType::ManualControl){
						sendAck();
					}
					m_state = WaitingForNextPacket;
					return Container::Result<Packet>(m_currentPacket);
				}
			}
		}
		else {
			m_state = WaitingForNextPacket;
			if(m_currentPacket.header.type != PacketType::Ack && m_currentPacket.header.type != PacketType::Nack) {
				sendAck();
			}

			return Container::Result<Packet>(m_currentPacket);
		}

		return Container::Result<Packet>();
	}

	bool Communication::checkHeaderCrc()
	{
		Crc crc = m_rfModule.read();

		if(crc != Packet::CalculateCRC8(m_currentPacket.header)) {
			TRACE("HEADER CRC ERROR RX_CRC = %d    ", crc);
			TRACE("CRC = %d\n\r", Packet::CalculateCRC8(m_currentPacket.contents));
			sendNack();
			m_state = WaitingForNextPacket;
			return false;
		}
		return true;
	}

	bool Communication::checkDataCrc()
	{
		Crc crc = m_rfModule.read();

		if(crc != Packet::CalculateCRC8(m_currentPacket.contents)) {
			TRACE("DATA CRC ERROR RX_CRC = %d    ", crc);
			TRACE("CRC = %d\n\r", Packet::CalculateCRC8(m_currentPacket.contents));
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
		TRACE("-> packet type[%d]\n\r", packet.header.type);
		sendHeader(packet.header);
		sendContents(packet.contents);
	}

	void Communication::send(PacketType::Enum type)
	{
		TRACE("-> packet type[%d]\n\r", type);
		PacketHeader header = {
				.id = m_transmitID++,
				.type = type
		};
		sendHeader(header);
	}

	void Communication::sendAck()
	{
		TRACE("-> packet ACK[%d]\n\r", PacketType::Ack);
		PacketHeader ack = {
				.id = m_currentPacket.header.id,
				.type = PacketType::Ack
		};
		sendHeader(ack);
	}

	void Communication::sendNack()
	{
		TRACE("-> packet NACK[%d]\n\r", PacketType::Nack);
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

