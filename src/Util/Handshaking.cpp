/*
 * Handshaking.cpp
 *
 *  Created on: 30.8.2019
 *      Author: Zahorack
 */


#include "Util/Trace.h"
#include "Util/Handshaking.h"


namespace Util {

Handshaking::Handshaking():
	m_timer(Util::Time::FromMilliSeconds(200)),
	m_state(HandshakingStates::Free)
{
	m_timer.start();
}

bool Handshaking::add(Control::Packet packet)
{
	if(packet.header.type == m_packetQueue.value().value.header.type) {
		return false;
	}

	return m_packetQueue.enqueue(packet);
}


Container::Result<Control::Packet> Handshaking::update()
{
	if(m_timer.run()) {
		return m_packetQueue.value();
	}

	return Container::Result<Control::Packet>();
}

bool Handshaking::check(Control::Packet ack)
{
	Container::Result<Control::Packet> current = m_packetQueue.value();

	if(ack.header.id == current.value.header.id) {
		m_packetQueue.dequeue();

		return true;
	}
	return false;
}





}
