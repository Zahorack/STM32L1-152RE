//
// Created by zahorack on 10/10/19.
//

#ifndef STM32L1_152RE_SONAR_H
#define STM32L1_152RE_SONAR_H

#include "Periph/Ultrasonic.h"
#include "Container/Result.h"
#include "Periph/JSNSR04.h"
#include "Util/Timer.h"

namespace Util {

typedef struct {
    Container::Result<Periph::ultrasonicResult_t> ultrasonicsResults[Periph::Ultrasonics::Size];
}sonarResult_t;

namespace SonarStates {
    enum Enum : uint8_t {
        SequenceTrigger = 0,

        None
    };
}

class Sonar {

    /* Multibeam array */
    Periph::Ultrasonic m_ultrasonics[Periph::Ultrasonics::Size] = {
        Periph::Ultrasonic(Periph::Ultrasonics::Ultrasonic2), Periph::Ultrasonic(Periph::Ultrasonics::Ultrasonic5),
        Periph::Ultrasonic(Periph::Ultrasonics::Ultrasonic4), Periph::Ultrasonic(Periph::Ultrasonics::Ultrasonic3),
        Periph::Ultrasonic(Periph::Ultrasonics::Ultrasonic1)
    };

    SonarStates::Enum m_state;
    Util::Timer m_timer;

public:
    Periph::JSNRS04 singleBeamUltrasonic;

    Sonar();
    ~Sonar();

    void update();
    bool available();
    bool available(Periph::Ultrasonics::Enum);

    void trigger(Periph::Ultrasonics::Enum);
    void trigger();

    void sequencedTrigger();

    Container::Result<Periph::ultrasonicResult_t> read(Periph::Ultrasonics::Enum);
    Container::Result<sonarResult_t> read();
};

}
#endif //STM32L1_152RE_SONAR_H
