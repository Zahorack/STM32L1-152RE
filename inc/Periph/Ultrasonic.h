/*
 *  Ultrasonic.h
 *
 *  Created on: 24.9.2019
 *      Author: Zahorack
 */

#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

#include "stm32l1xx.h"
#include "Util/Timer.h"
#include "Periph/SysTickCounter.h"
#include "Container/Result.h"

namespace Periph {

namespace Ultrasonics {
enum Enum : uint8_t {
	Ultrasonic1 = 0,
	Ultrasonic2,
	Ultrasonic3,
	Ultrasonic4,
	Ultrasonic5,

	Size
};
}

namespace UltrasonicStates {
enum Enum : uint8_t {
	Echo = 0,
	Trigger,
	Silent,

	None
};
}

namespace UltrasonicWaves {
enum Enum : uint8_t {
    Echo1 = 0,
    Echo2,
    Echo3,
    Echo4,
    Echo5,
    Echo6,
    Echo7,
    Echo8,
    Echo9,

    Size
};
}
typedef struct {
    uint64_t echoTime;
    uint64_t endTime;
} wavesArgs_t;

typedef struct {
    uint32_t echoInterval[UltrasonicWaves::Size];
    uint32_t pulseInterval[UltrasonicWaves::Size];
}ultrasonicResult_t;


typedef struct {
    float x_est_last = 0;
    float P_last = 0;
} kalmanArgs_t;

namespace Edges {
    enum  Enum : uint8_t {
        Rising = 0,
        Falling
    };
}

typedef struct {
    bool data_ready;
    Edges::Enum last_edge;
    uint64_t triggerTime;
    wavesArgs_t waves[UltrasonicWaves::Size];
    uint8_t waveIndex;
    kalmanArgs_t kalman_args;
} ultrasonicArgs_t;


class Ultrasonic {

	const Ultrasonics::Enum m_id;
    UltrasonicStates::Enum m_state;

	Util::Timer m_timer;
	Periph::SysTickCounter m_sysTick;

	void initRcc();

	void evaluate(Container::Result<ultrasonicResult_t> *, UltrasonicWaves::Enum);

public:
	Ultrasonic(Ultrasonics::Enum id);

	void update();
	void configure(UltrasonicStates::Enum fcn);

	void trigger();
	bool available();

	Container::Result<ultrasonicResult_t> read();


};

}


#endif