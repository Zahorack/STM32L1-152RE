/*
 * Handshaking.h
 *
 *  Created on: 30.8.2019
 *      Author: Zahorack
 */

#ifndef HANDSHAKING_H_
#define HANDSHAKING_H_

#include "stm32l1xx.h"
#include "Util/Timer.h"
#include "Control/Packet.h"
#include "Container/Queue.h"
#include "Container/Result.h"


namespace Util {

namespace HandshakingStates {
enum Enum : uint8_t {
	Busy = 0,
	Free
};
}

class Handshaking {

	Container::Queue<Control::Packet, 10> m_packetQueue;
	Util::Timer m_timer;
	uint8_t m_state;

public:
	Handshaking();

	bool add(Control::Packet);

	Container::Result<Control::Packet> update();

	bool check(Control::Packet);

};

}

#endif
