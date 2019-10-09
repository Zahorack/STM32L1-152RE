/*
 *  FineTimer.h
 *
 *  Created on: 24.9.2019
 *      Author: Zahorack
 */

#ifndef FINETIMER_H_
#define FINETIMER_H_

#include "stm32l1xx.h"

namespace Periph {


class FineTimer {

public:
	FineTimer();

	uint64_t read();
	void update();



};

}


#endif