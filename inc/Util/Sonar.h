//
// Created by zahorack on 10/10/19.
//

#ifndef STM32L1_152RE_SONAR_H
#define STM32L1_152RE_SONAR_H

#include "Periph/Ultrasonic.h"
#include "Container/Result.h"

namespace Util {

typedef struct {
    Container::Result<Periph::ultrasonicResult_t> ultrasonicsResults[Periph::Ultrasonics::Size];
}sonarResult_t;

class Sonar {

    Periph::Ultrasonic m_ultrasonics[Periph::Ultrasonics::Size] = {
        Periph::Ultrasonic(Periph::Ultrasonics::Ultrasonic2), Periph::Ultrasonic(Periph::Ultrasonics::Ultrasonic5),
        Periph::Ultrasonic(Periph::Ultrasonics::Ultrasonic4), Periph::Ultrasonic(Periph::Ultrasonics::Ultrasonic3),
        Periph::Ultrasonic(Periph::Ultrasonics::Ultrasonic1)
    };

public:
    Sonar();
    ~Sonar();

    void update();
    bool available();

    void trigger(Periph::Ultrasonics::Enum);
    void trigger();

    Container::Result<sonarResult_t> read();
};

}
#endif //STM32L1_152RE_SONAR_H
