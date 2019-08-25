/*
 * AdcIT.h
 *
 *  Created on: 24.8.2019
 *      Author: Zahorack
 */

#ifndef ADCIT_H_
#define ADCIT_H_

#include "stm32l1xx.h"

namespace Periph {


namespace AdcITPins {
enum Enum : uint8_t {
	AdcPin1 = 0,
	AdcPin2,

	Size
};
}

namespace AdcFlags{
enum Enum : uint8_t {
	dataBussy = 0,
	dataReady
};
}

class AdcIT {
	const AdcITPins::Enum id;

	void initGpio();
	void initRCC();
	void initAdc();

public:
	AdcIT(AdcITPins::Enum a_id);

	uint32_t getData();

	void startRead();

	bool isReady();
};

}

#endif