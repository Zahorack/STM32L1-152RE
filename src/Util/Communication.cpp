/*
 * Comunication.cpp
 *
 *  Created on: 2.9. 2018
 *      Author: Zahorack
 */

#include "Util/Communication.h"
#include "Util/Trace.h"

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

	void Communication::sendStatus()
	{
		m_rfModule.write("STATUS\n");
	}

	void Communication::waitForNextPacket()
	{
		while(m_rfModule.bytesAvailable() > sizeof(Packet::Mark)) {
			if(m_rfModule.readWord() == Packet::Mark) {
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
				m_state = ReadingPacketContents;
			}
			else {
				TRACE("Header CRC ERROR\n\r");
			}
		}
	}

	Container::Result<Packet> Communication::readPacketContents()
	{
		if(m_rfModule.bytesAvailable() >= Packet::SizeForType(m_currentPacket.header.type) + sizeof(Crc)) {
			//m_rfModule.readStruct(m_currentPacket.contents);
			m_rfModule.readBytes(reinterpret_cast<uint8_t *>(&m_currentPacket.contents), Packet::SizeForType(m_currentPacket.header.type));

			if(/*Packet::SizeForType(m_currentPacket.header.type) && */checkDataCrc()) {
				m_state = WaitingForNextPacket;
				return Container::Result<Packet>(m_currentPacket);
			}
		}

		return Container::Result<Packet>();
	}

	bool Communication::checkHeaderCrc()
	{
		Crc crc = m_rfModule.read();

		if(crc != Packet::CalculateCRC8(m_currentPacket.header)) {
			const PacketHeader nackPacket = {
					.id = m_currentPacket.header.id,
					.type = PacketType::Nack
			};

			m_rfModule.writeStruct(nackPacket);

			m_state = WaitingForNextPacket;

			return false;
		}

		return true;
	}

	bool Communication::checkDataCrc()
	{
		Crc crc = m_rfModule.read();

		if(crc != Packet::CalculateCRC8(m_currentPacket.contents.dataPacket)) {
			TRACE("DATA CRC ERRO -- RX CRC = %d   ", crc);
			TRACE("CRC = %d\n\r", Packet::CalculateCRC8(m_currentPacket.contents.dataPacket));

			const PacketHeader nackPacket = {
					.id = m_currentPacket.header.id,
					.type = PacketType::Nack
			};

			m_rfModule.writeStruct(nackPacket);

			m_state = WaitingForNextPacket;

			return false;
		}

		return true;
	}
}

//
//namespace Util {
//
//Communication::Communication() :
//	m_rfModule(Periph::Usarts::Usart2, 9600)
//{}
//
//void Communication::start(){}
//
//void Communication::run()
//{
//	if(m_rfModule.bytesAvailable() >= 2){
//		uint16_t mark = m_rfModule.readWord();
//
//		if(mark == m_packet.mark){
//			m_packet.parse();
//		}
//	}
//
//
//}
//
//void Communication::parse(){
//
//	if(m_rfModule.bytesAvailable() >= sizeof(m_packet.header -2)){
//		m_rfModule.readBytes((uint8_t *)&m_packet.header +2, sizeof(m_packet.header -2));
//	}
//
//	switch (m_packet.header->type){
//
//	case packet_type_status :  break;
//	case packet_type_bussy :  break;
//	case packet_type_error :  break;
//	case packet_type_ack :  break;
//	case packet_type_data_ack :  break;
//	case packet_type_header_ack :  break;
//	case packet_type_controller_data :  break;
//	case packet_type_move_data :  break;
//	case packet_type_print_data :  break;
//	case packet_type_nack :  break;
//
//	default:
//		DTRACE("BAD PACKET TYPE !");
//		break;
//	}
//}
//
//}
