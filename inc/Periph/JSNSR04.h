//
// Created by zahorack on 5/6/20.
//

#ifndef STM32L1_152RE_JSNSR04_H
#define STM32L1_152RE_JSNSR04_H

#include "stm32l1xx.h"
#include "Ultrasonic.h"

namespace Periph {

typedef struct {
    uint32_t echoInterval;
}jsnsr04Result_t;

namespace JSNSR04_Sensors {
    enum Enum : uint8_t {
        Sensor1 = 0,

        Size
    };
}

class JSNRS04 {

    const JSNSR04_Sensors::Enum m_id;
    UltrasonicStates::Enum m_state;

    void initRcc();
    void initGpio();


public:
    JSNRS04(JSNSR04_Sensors::Enum id);

    void update();
    void triggerDown();
    void triggerUp();

    void trigger();
    bool available();

    Container::Result<jsnsr04Result_t> read();

};

}

#endif //STM32L1_152RE_JSNSR04_H
