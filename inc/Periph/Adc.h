/*
 * Adc.h
 *
 *  Created on: 24.8.2019
 *      Author: Zahorack
 */

#ifndef ADC_H_
#define ADC_H_

#include "stm32l1xx.h"

namespace Periph {


namespace AdcPins {
enum Enum : uint8_t {
	AdcPin1 = 0,
	AdcPin2,

	Size
};
}

class Adc {
	const AdcPins::Enum id;

	void initGpio();
	void initRCC();
	void initAdc();
public:
	Adc(AdcPins::Enum a_id);

	uint16_t read();

};

}

#endif